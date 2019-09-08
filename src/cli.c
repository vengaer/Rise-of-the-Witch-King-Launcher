#include "cli.h"
#include "config.h"
#include "command.h"
#include "fsys.h"
#include <ctype.h>
#include <omp.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

extern void(*display_error)(char const*);
extern void(*display_errorf)(char const*, ...);

void print_help(void) {
    fprintf(stderr, "Usage: rotwkl [OPTION]\n\n");
    fprintf(stderr, "    -r <config>,        Run the given configuration.\n");
    fprintf(stderr, "    -s <config>,        Specify what configuration to enable.\n");
    fprintf(stderr, "    -u <config>,        Update config file for specified configuration.\n");
    fprintf(stderr, "    -c <path>,          Specify path to launcher config file.\n");
    fprintf(stderr, "    -h,                 Display this help message.\n");
    fprintf(stderr, "Available configurations are:\n");
    fprintf(stderr, "    rotwk\n    edain(*)\n    botta(*)\n, all(**)\n");
    fprintf(stderr, "*  The mod to enable must be installed correctly\n");
    fprintf(stderr, "** Available only with the -u flag\n");
}

static int get_launcher_dir(char* dst, char const* restrict launcher_path) {
    char arg[PATH_SIZE];

    if(strscpy(arg, launcher_path, sizeof arg) < 0)
        return -E2BIG;

    replace_char(arg, '\\', '/');

    parent_path(dst, arg);
    return 0;
}

void cli_error_diag(char const* info) {
    fprintf(stderr, "\nError: %s\n\n", info);
}

void cli_error_diagf(char const* fmt, ...) {
    char buf[LINE_SIZE];

    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    cli_error_diag(buf);
}

int cli_main(int argc, char** argv) {
    char config_file[PATH_SIZE];
    char launcher_dir[PATH_SIZE];

    bool r_flag = false, s_flag = false, u_flag = false, h_flag = false;
    char *scfg = NULL, *ucfg = NULL;
    int idx, opt, sync;
    bool mounting_necessary = true; 
    bool new_dat_enabled;
    struct latch latch;

    char rotwk_toml[PATH_SIZE];
    char edain_toml[PATH_SIZE];
    char botta_toml[PATH_SIZE];
    char launch_cmd[PATH_SIZE];
    char launch[PATH_SIZE];
    char dat_file[PATH_SIZE];
    char game_csum[ENTRY_SIZE];

    struct launcher_data ld;

    if(get_launcher_dir(launcher_dir, argv[0]) < 0) {
        display_error("Could not get launcher directory\n");
        return 1;
    }

    #if defined __GNUC__ && !defined __clang__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wformat-truncation"
    #endif
    snprintf(config_file, sizeof config_file, "%stoml/launcher.toml", launcher_dir);
    #if defined __GNUC__ && !defined __clang__
    #pragma GCC diagnostic pop
    #endif

    char const* lcfg = config_file;

    while((opt = getopt(argc, argv, ":r:s:u:c:hnv")) != -1) {
        switch(opt) {
            case 'r':
                if(!s_flag && !r_flag) {
                    r_flag = true;
                    scfg = optarg;
                }
                break;
            case 's':
                if(!r_flag && !s_flag) {
                    s_flag = true;
                    scfg = optarg;
                }
                break;
            case 'u':
                u_flag = true;
                ucfg = optarg;
                break;
            case 'c':
                lcfg = optarg;
                break;
            case 'h':
                h_flag = true;
                break;
            case '?':
                if(optopt == 'r' || optopt == 's' || optopt == 'u')
                    display_errorf("Option -%c requires an argument\n", optopt);
                else if(isprint(optopt))
                    display_errorf("Unknown option '-%c'\n", optopt);
                else
                    display_errorf("Unknown option character '\\x%x'\n", optopt);
                return 1;
            default:
                return 1;
        }
    }

    for(idx = optind; idx < argc; idx++)
        display_errorf("Non-option argument %s ignored\n", argv[idx]);

    if(h_flag) {
        print_help();
        return 1;
    }

    if(!file_exists(lcfg)) {
        display_error("No config file found, using default values\n");
        launcher_data_init(&ld);
    }
    else  {
        if(!read_launcher_config(&ld, lcfg)) {
            display_error("Failed to read launcher config\n");
            return 1;
        }
    }

    show_console(ld.show_console);

    if(strscpy(rotwk_toml, launcher_dir, sizeof rotwk_toml) < 0) {
        display_error("Launcher path overflowed the rotwk toml buffer\n");
        return 1;
    }
    if(strscat(rotwk_toml, "/toml/rotwk.toml", sizeof rotwk_toml) < 0) {
        display_error("rotwkl toml overflowed the buffer\n");
        return 1;
    }

    if(strscpy(edain_toml, launcher_dir, sizeof edain_toml) < 0) {
        display_error("Launcher path overflowed the edain toml buffer\n");
        return 1;
    }
    if(strscat(edain_toml, "/toml/edain.toml", sizeof edain_toml) < 0) {
        display_error("Edain toml overflowed the buffer\n");
        return 1;
    }

    if(strscpy(botta_toml, launcher_dir, sizeof botta_toml) < 0) {
        display_error("Launcher path overflowed the botta toml buffer\n");
        return 1;
    }
    if(strscat(botta_toml, "/toml/botta.toml", sizeof botta_toml) < 0) {
        display_error("Botta toml overflowed the buffer\n");
        return 1;
    }

    chdir(ld.game_path);

    if(strscpy(launch_cmd, ld.game_path, sizeof launch_cmd) < 0) {
        display_error("Game path overflowed the launch command buffer\n");
        return 1;
    }
    if(strscat(launch_cmd, "/lotrbfme2ep1.exe", sizeof launch_cmd) < 0) {
        display_error("Exe path overflowed the launch command buffer\n");
        return 1;
    }

    if(strscpy(dat_file, ld.game_path, sizeof dat_file) < 0) {
        display_error("Game path overflowed the game.dat buffer\n");
        return 1;
    }
    if(strscat(dat_file, "/game.dat", sizeof dat_file) < 0) {
        display_error("game.dat path overflowed the buffer\n");
        return 1;
    }

    md5sum(dat_file, game_csum);
    new_dat_enabled = strcmp(game_csum, NEW_DAT_CSUM) == 0;

    /* Update */
    sync = 1;
    if(ucfg && strcmp(ucfg, "all") == 0) {
        if(ld.edain_available)
            ++sync;
        if(ld.botta_available)
            ++sync;
    }
    latch_init(&latch, sync);

    if(ucfg && u_flag) {
        if(strcmp(ucfg, "rotwk") == 0) 
            update_game_config(rotwk_toml, !new_dat_enabled, &latch, &ld);
        else if(strcmp(ucfg, "edain") == 0) {

            if(!ld.edain_available) {
                display_error("Edain is not avaialble\n");
                return 1;
            }

            update_game_config(edain_toml, new_dat_enabled, &latch, &ld);
        }
        else if(ld.botta_available && strcmp(ucfg, "botta") == 0) {

            if(!ld.botta_available) {
                display_error("BotTA is not available\n");
                return 1;
            }

            update_game_config(botta_toml, new_dat_enabled, &latch, &ld);
        }
        else if(strcmp(ucfg, "all") == 0) {
            #pragma omp parallel num_threads(3)
            {
                #pragma omp master
                {
                    prepare_progress();

                    #pragma omp task if(ld.edain_available)
                        update_game_config(edain_toml, new_dat_enabled, &latch, &ld);

                    #pragma omp task if(ld.botta_available)
                        update_game_config(botta_toml, new_dat_enabled, &latch, &ld);

                    update_game_config(rotwk_toml, !new_dat_enabled, &latch, &ld);

                    reset_progress();
                }
            }
        }
        else {
            display_error("Unknown configuration\n");
            return 1;
        }
    }

    enum configuration active_config;

    if(r_flag || (scfg && s_flag)) {
        /* Set active config */
        if(strcmp(scfg, "rotwk") == 0) {
            set_active_configuration(rotwk_toml, ld.patch_version, true, ld.verify_active);
            active_config = rotwk;
        }
        else {
            if(strcmp(scfg, "edain") == 0) {
                if(!ld.edain_available) {
                    display_error("Edain is not available\n");
                    return 1;
                }

                set_active_configuration(edain_toml, ld.patch_version, ld.swap_dat_file, ld.verify_active);

                active_config = edain;
            }
            else if(ld.botta_available && strcmp(scfg, "botta") == 0) {
                if(!ld.botta_available) {
                    display_error("BotTA is not avaialble\n");
                    return 1;
                }

                set_active_configuration(botta_toml, ld.patch_version, ld.swap_dat_file, ld.verify_active);
                active_config = botta;
            }
            else {
                display_error("Unknown configuration\n");
                return 1;
            }
        }

        /* Launch */
        if(r_flag) {
            if(ld.automatic_mount) {
                md5sum(dat_file, game_csum);
                mounting_necessary = strcmp(game_csum, NEW_DAT_CSUM);

                if(mounting_necessary) {
                    if(system(ld.mount_cmd) != 0) {
                        display_errorf("'%s' returned an error\n", ld.mount_cmd);
                        return 1;
                    }
                }
            }

            if(active_config == botta) {
                if(strscpy(launch_cmd, ld.botta_path, sizeof launch_cmd) < 0) {
                    display_error("Botta path overflowed the launch buffer\n");
                    /* Unmount if necessary */
                    if(ld.automatic_mount && mounting_necessary) {
                        if(system(ld.umount_cmd) != 0)
                            display_errorf("'%s' returned an error\n", ld.umount_cmd);
                    }
                    return 1;
                }
                if(strscat(launch_cmd, "/BotTa.lnk", sizeof launch_cmd) < 0) {
                    display_error("Botta lnk overflowed the buffer\n");
                    /* Unmount if necessary */
                    if(ld.automatic_mount && mounting_necessary) {
                        if(system(ld.umount_cmd) != 0)
                            display_errorf("'%s' returned an error\n", ld.umount_cmd);
                    }
                    return 1;
                }
            }

            sys_format(launch, launch_cmd);

            if(system(launch) != 0)
                display_error("Failed to launch game\n");

            while(game_running())
                sleep_for(SLEEP_TIME);

            switch(ld.default_state) {
                case rotwk:
                    set_active_configuration(rotwk_toml, ld.patch_version, true, false);
                    break;
                case edain:
                    set_active_configuration(edain_toml, ld.patch_version, ld.swap_dat_file, false);
                    break;
                case botta:
                    set_active_configuration(botta_toml, ld.patch_version, ld.swap_dat_file, false);
                    break;
                default:
                    break;
            }

            if(ld.automatic_mount && mounting_necessary) {
                if(system(ld.umount_cmd) != 0) {
                    display_errorf("'%s' returned an error\n", ld.umount_cmd);
                    return 1;
                }
            }
        }
    }

    return 0;
}

