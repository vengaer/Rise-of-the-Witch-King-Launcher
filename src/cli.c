#include "cli.h"
#include "config.h"
#include "command.h"
#include "fsys.h"
#include "strutils.h"
#include <ctype.h>
#include <omp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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

int get_launcher_dir(char* dst, char const* launcher_path) {
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
        fprintf(stderr, "Could not get launcher directory\n");
        return 1;
    }
    snprintf(config_file, sizeof config_file, "%stoml/launcher.toml", launcher_dir);

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
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                else if(isprint(optopt))
                    fprintf(stderr, "Unknown option '-%c'.\n", optopt);
                else
                    fprintf(stderr, "Unknown option character '\\x%x'.\n", optopt);
                return 1;
            default:
                return 1;
        }
    }

    for(idx = optind; idx < argc; idx++)
        printf("Non-option argument %s ignored.\n", argv[idx]);

    if(h_flag) {
        print_help();
        return 1;
    }

    if(!file_exists(lcfg)) {
        fprintf(stderr, "Warning: no config file found. Using default values\n");
        launcher_data_init(&ld);
    }
    else  {
        if(!read_launcher_config(&ld, lcfg)) {
            fprintf(stderr, "Failed to read launcher config, terminating\n");
            return 1;
        }
    }

    show_console(ld.show_console);

    if(strscpy(rotwk_toml, launcher_dir, sizeof rotwk_toml) < 0) {
        fprintf(stderr, "Launcher path overflowed the rotwk toml buffer\n");
        return 1;
    }
    if(strscat(rotwk_toml, "/toml/rotwk.toml", sizeof rotwk_toml) < 0) {
        fprintf(stderr, "rotwk toml overflowed the buffer\n");
        return 1;
    }

    if(strscpy(edain_toml, launcher_dir, sizeof edain_toml) < 0) {
        fprintf(stderr, "Launcher path overflowed the edain toml buffer\n");
        return 1;
    }
    if(strscat(edain_toml, "/toml/edain.toml", sizeof edain_toml) < 0) {
        fprintf(stderr, "edain toml overflowed the buffer\n");
        return 1;
    }

    if(strscpy(botta_toml, launcher_dir, sizeof botta_toml) < 0) {
        fprintf(stderr, "Launcher path overflowed the botta toml buffer\n");
        return 1;
    }
    if(strscat(botta_toml, "/toml/botta.toml", sizeof botta_toml) < 0) {
        fprintf(stderr, "botta toml overflowed the buffer\n");
        return 1;
    }

    chdir(ld.game_path);

    if(strscpy(launch_cmd, ld.game_path, sizeof launch_cmd) < 0) {
        fprintf(stderr, "Game path overflowed the launch command buffer\n");
        return 1;
    }
    if(strscat(launch_cmd, "/lotrbfme2ep1.exe", sizeof launch_cmd) < 0) {
        fprintf(stderr, "Exe path overflowed the launch command buffer\n");
        return 1;
    }

    if(strscpy(dat_file, ld.game_path, sizeof dat_file) < 0) {
        fprintf(stderr, "Game path overflowed the game.dat buffer\n");
        return 1;
    }
    if(strscat(dat_file, "/game.dat", sizeof dat_file) < 0) {
        fprintf(stderr, "game.dat path overflowed the buffer\n");
        return 1;
    }
    strncat(dat_file, "/game.dat", sizeof dat_file - strlen(ld.game_path) - 1);

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
                fprintf(stderr, "Edain is not available\n");
                return 1;
            }

            update_game_config(edain_toml, new_dat_enabled, &latch, &ld);
        }
        else if(ld.botta_available && strcmp(ucfg, "botta") == 0) {

            if(!ld.botta_available) {
                fprintf(stderr, "BotTA is not available\n");
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
            fprintf(stderr, "Unknown configuration %s\n", ucfg);
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
                    fprintf(stderr, "Edain is not available\n");
                    return 1;
                }

                set_active_configuration(edain_toml, ld.patch_version, ld.swap_dat_file, ld.verify_active);

                active_config = edain;
            }
            else if(ld.botta_available && strcmp(scfg, "botta") == 0) {
                if(!ld.botta_available) {
                    fprintf(stderr, "BotTA is not available\n");
                    return 1;
                }

                set_active_configuration(botta_toml, ld.patch_version, ld.swap_dat_file, ld.verify_active);
                active_config = botta;
            }
            else {
                fprintf(stderr, "Unknown configuration %s\n", scfg);
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
                        fprintf(stderr, "'%s' returned an error\n", ld.mount_cmd);
                        return 1;
                    }
                }
            }

            if(active_config == botta) {
                if(strscpy(launch_cmd, ld.botta_path, sizeof launch_cmd) < 0) {
                    fprintf(stderr, "Botta path overflowed the launch buffer\n");
                    /* Unmount if necessary */
                    if(ld.automatic_mount && mounting_necessary) {
                        if(system(ld.umount_cmd) != 0)
                            fprintf(stderr, "'%s' returned an error\n", ld.umount_cmd);
                    }
                    return 1;
                }
                if(strscat(launch_cmd, "/BotTa.lnk", sizeof launch_cmd) < 0) {
                    fprintf(stderr, "Botta lnk overflowed the buffer\n");
                    /* Unmount if necessary */
                    if(ld.automatic_mount && mounting_necessary) {
                        if(system(ld.umount_cmd) != 0)
                            fprintf(stderr, "'%s' returned an error\n", ld.umount_cmd);
                    }
                    return 1;
                }
            }

            sys_format(launch, launch_cmd);

            if(system(launch) != 0)
                fprintf(stderr, "Failed to launch game.\n");

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
                    fprintf(stderr, "'%s' returned an error\n", ld.umount_cmd);
                    return 1;
                }
            }
        }
    }

    return 0;
}

