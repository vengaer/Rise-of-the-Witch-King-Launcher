#include "cli.h"
#include "config.h"
#include "command.h"
#include "fsys.h"
#include "progress_callback.h"
#include <ctype.h>
#include <omp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

extern void(*display_error)(char const*);

static void print_help(void);
static int get_launcher_dir(char* restrict dst, char const* restrict launcher_path);
static bool setup_config(struct launcher_data* ld, char const* launch_cfg);
static bool construct_rotwkl_toml_path(char* restrict dst, char const* restrict launcher_dir, size_t dst_size);
static bool construct_edain_toml_path(char* restrict dst, char const* restrict launcher_dir, size_t dst_size);
static bool construct_botta_toml_path(char* restrict dst, char const* restrict launcher_dir, size_t dst_size);
static bool construct_launch_cmd(char* restrict dst, char const* restrict game_path, size_t dst_size);
static bool construct_botta_launch_cmd(char* restrict dst, char const* restrict botta_path, size_t dst_size);
static bool construct_dat_file_path(char* restrict dst, char const* restrict game_path, size_t dst_size);
static bool update(char const* restrict upd_cfg, bool new_dat_enabled, struct launcher_data const* ld,
                   char const* restrict edain_toml, char const* restrict botta_toml, char const* restrict rotwk_toml);
static int set(char const* restrict set_cfg, struct launcher_data const* ld, char const* restrict edain_toml,
                char const* restrict botta_toml, char const* restrict rotwk_toml);
static bool launch(char* restrict launch_cmd, size_t launch_cmd_size, char const* restrict dat_file, struct launcher_data const* ld, 
                   enum configuration active_config, char const* restrict edain_toml, char const* restrict botta_toml, char const* restrict rotwk_toml);

void cli_error_diag(char const* info) {
    fprintf(stderr, "\nError: %s\n\n", info);
}

int cli_main(int argc, char** argv) {
    char config_file[PATH_SIZE];
    char launcher_dir[PATH_SIZE];

    bool run_flag = false, set_flag = false, upd_flag = false, h_flag = false;
    char *set_cfg = NULL, *upd_cfg = NULL;
    int idx, opt;
    bool new_dat_enabled;

    char rotwk_toml[PATH_SIZE];
    char edain_toml[PATH_SIZE];
    char botta_toml[PATH_SIZE];
    char launch_cmd[PATH_SIZE];
    char dat_file[PATH_SIZE];
    char game_csum[ENTRY_SIZE];

    struct launcher_data ld;
    char const* launch_cfg = config_file;

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


    while((opt = getopt(argc, argv, ":r:s:u:c:hnv")) != -1) {
        switch(opt) {
            case 'r':
                if(!set_flag && !run_flag) {
                    run_flag = true;
                    set_cfg = optarg;
                }
                break;
            case 's':
                if(!run_flag && !set_flag) {
                    set_flag = true;
                    set_cfg = optarg;
                }
                break;
            case 'u':
                upd_flag = true;
                upd_cfg = optarg;
                break;
            case 'c':
                launch_cfg = optarg;
                break;
            case 'h':
                h_flag = true;
                break;
            case '?':
                if(optopt == 'r' || optopt == 's' || optopt == 'u')
                    errorfmt("Option -%c requires an argument\n", optopt);
                else if(isprint(optopt))
                    errorfmt("Unknown option '-%c'\n", optopt);
                else
                    errorfmt("Unknown option character '\\x%x'\n", optopt);
                return 1;
            default:
                return 1;
        }
    }

    for(idx = optind; idx < argc; idx++)
        errorfmt("Non-option argument %s ignored\n", argv[idx]);

    if(h_flag) {
        print_help();
        return 1;
    }

    if(!setup_config(&ld, launch_cfg))
        return 1;

    show_console(ld.show_console);

    if(!construct_rotwkl_toml_path(rotwk_toml, launcher_dir, sizeof rotwk_toml))
        return 1;
    if(!construct_edain_toml_path(edain_toml, launcher_dir, sizeof edain_toml))
        return 1;
    if(!construct_botta_toml_path(botta_toml, launcher_dir, sizeof botta_toml))
        return 1;

    chdir(ld.game_path);

    if(!construct_launch_cmd(launch_cmd, ld.game_path, sizeof launch_cmd))
        return 1;
    if(!construct_dat_file_path(dat_file, ld.game_path, sizeof dat_file))
        return 1;

    md5sum(dat_file, game_csum);
    new_dat_enabled = strcmp(game_csum, NEW_DAT_CSUM) == 0;

    /* Update */
    if(upd_cfg && upd_flag) {
        if(!update(upd_cfg, new_dat_enabled, &ld, edain_toml, botta_toml, rotwk_toml))
            return 1;
    }

    enum configuration active_config = ld.default_state;
    if(set_cfg && set_flag) {
        active_config = set(set_cfg, &ld, edain_toml, botta_toml, rotwk_toml);
        if(active_config < 0)
            return 1;
    }

    if(run_flag) {
        if(!launch(launch_cmd, sizeof launch_cmd, dat_file, &ld, active_config, edain_toml, botta_toml, rotwk_toml))
            return 1;
    }

    return 0;
}

static void print_help(void) {
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

static int get_launcher_dir(char* restrict dst, char const* restrict launcher_path) {
    char arg[PATH_SIZE];

    if(strscpy(arg, launcher_path, sizeof arg) < 0)
        return -E2BIG;

    replace_char(arg, '\\', '/');

    parent_path(dst, arg);
    return 0;
}

static bool setup_config(struct launcher_data* ld, char const* launch_cfg) {
    if(!file_exists(launch_cfg)) {
        display_error("No config file found, using default values\n");
        launcher_data_init(ld);
    }
    else  {
        if(!read_launcher_config(ld, launch_cfg)) {
            display_error("Failed to read launcher config\n");
            return false;
        }
    }
    return true;
}

static bool construct_rotwkl_toml_path(char* restrict dst, char const* restrict launcher_dir, size_t dst_size) {
    if(strscpy(dst, launcher_dir, dst_size) < 0) {
        display_error("Launcher path overflowed the rotwk toml buffer\n");
        return false;
    }
    if(strscat(dst, "/toml/rotwk.toml", dst_size) < 0) {
        display_error("rotwkl toml overflowed the buffer\n");
        return false;
    }
    return true;
}

static bool construct_edain_toml_path(char* restrict dst, char const* restrict launcher_dir, size_t dst_size) {
    if(strscpy(dst, launcher_dir, dst_size) < 0) {
        display_error("Launcher path overflowed the edain toml buffer\n");
        return false;
    }
    if(strscat(dst, "/toml/edain.toml", dst_size) < 0) {
        display_error("Edain toml overflowed the buffer\n");
        return false;
    }
    return true;
}

static bool construct_botta_toml_path(char* restrict dst, char const* restrict launcher_dir, size_t dst_size) {
    if(strscpy(dst, launcher_dir, dst_size) < 0) {
        display_error("Launcher path overflowed the botta toml buffer\n");
        return false;
    }
    if(strscat(dst, "/toml/botta.toml", dst_size) < 0) {
        display_error("Botta toml overflowed the buffer\n");
        return false;
    }
    return true;
}

static bool construct_launch_cmd(char* restrict dst, char const* restrict game_path, size_t dst_size) {
    if(strscpy(dst, game_path, dst_size) < 0) {
        display_error("Game path overflowed the launch command buffer\n");
        return false;
    }
    if(strscat(dst, "/lotrbfme2ep1.exe", dst_size) < 0) {
        display_error("Exe path overflowed the launch command buffer\n");
        return false;
    }
    return true;
}

static bool construct_botta_launch_cmd(char* restrict dst, char const* restrict botta_path, size_t dst_size) {
    if(strscpy(dst, botta_path, dst_size) < 0) {
        display_error("Botta path overflowed the launch buffer\n");
        return false;
    }
    if(strscat(dst, "/BotTa.lnk", dst_size) < 0) {
        display_error("Botta lnk overflowed the buffer\n");
        return false;
    }
    return true;
}

static bool construct_dat_file_path(char* restrict dst, char const* restrict game_path, size_t dst_size) {
    if(strscpy(dst, game_path, dst_size) < 0) {
        display_error("Game path overflowed the game.dat buffer\n");
        return false;
    }
    if(strscat(dst, "/game.dat", dst_size) < 0) {
        display_error("game.dat path overflowed the buffer\n");
        return false;
    }
    return true;
}

static bool update(char const* restrict upd_cfg, bool new_dat_enabled, struct launcher_data const* ld,
                   char const* restrict edain_toml, char const* restrict botta_toml, char const* restrict rotwk_toml) {
    unsigned sync;
    struct latch latch;
    struct progress_callback pc;
    progress_init(&pc);

    sync = 1;
    if(upd_cfg && strcmp(upd_cfg, "all") == 0) {
        if(ld->edain_available)
            ++sync;
        if(ld->botta_available)
            ++sync;
    }
    latch_init(&latch, sync);

    /* Only rotwk */
    if(strcmp(upd_cfg, "rotwk") == 0) 
        update_game_config(rotwk_toml, !new_dat_enabled, &latch, ld, &pc);
    /* Only edain */
    else if(strcmp(upd_cfg, "edain") == 0) {

        if(!ld->edain_available) {
            display_error("Edain is not avaialble\n");
            return false;
        }

        update_game_config(edain_toml, new_dat_enabled, &latch, ld, &pc);
    }
    /* Only botta */
    else if(strcmp(upd_cfg, "botta") == 0) {

        if(!ld->botta_available) {
            display_error("BotTA is not available\n");
            return false;
        }

        update_game_config(botta_toml, new_dat_enabled, &latch, ld, &pc);
    }
    /* All */
    else if(strcmp(upd_cfg, "all") == 0) {
        #pragma omp parallel num_threads(3)
        {
            #pragma omp master
            {
                #pragma omp task if(ld->edain_available)
                    update_game_config(edain_toml, new_dat_enabled, &latch, ld, &pc);

                #pragma omp task if(ld->botta_available)
                    update_game_config(botta_toml, new_dat_enabled, &latch, ld, &pc);

                update_game_config(rotwk_toml, !new_dat_enabled, &latch, ld, &pc);
            }
        }
    }
    else {
        errorfmt("Unknown configuration %s\n", upd_cfg);
        return false;
    }
    return true;
}

static int set(char const* restrict set_cfg, struct launcher_data const* ld, char const* restrict edain_toml,
                char const* restrict botta_toml, char const* restrict rotwk_toml) {
    enum configuration active_config;

    if(strcmp(set_cfg, "rotwk") == 0) {
        set_active_configuration(rotwk_toml, ld->patch_version, true, ld->verify_active);
        active_config = rotwk;
    }
    else {
        if(strcmp(set_cfg, "edain") == 0) {
            if(!ld->edain_available) {
                display_error("Edain is not available\n");
                return -1;
            }

            set_active_configuration(edain_toml, ld->patch_version, ld->swap_dat_file, ld->verify_active);

            active_config = edain;
        }
        else if(strcmp(set_cfg, "botta") == 0) {
            if(!ld->botta_available) {
                display_error("BotTA is not avaialble\n");
                return -1;
            }

            set_active_configuration(botta_toml, ld->patch_version, ld->swap_dat_file, ld->verify_active);
            active_config = botta;
        }
        else {
            display_error("Unknown configuration\n");
            return -1;
        }
    }
    return active_config;
}

static bool launch(char* restrict launch_cmd, size_t launch_cmd_size, char const* restrict dat_file, struct launcher_data const* ld, 
                   enum configuration active_config, char const* restrict edain_toml, char const* restrict botta_toml, char const* restrict rotwk_toml) {
    char csum[ENTRY_SIZE];
    char launch_call[PATH_SIZE];
    bool mounting_necessary = true;

    if(ld->automatic_mount) {
        md5sum(dat_file, csum);
        mounting_necessary = strcmp(csum, NEW_DAT_CSUM);

        if(mounting_necessary) {
            if(system(ld->mount_cmd) != 0) {
                errorfmt("'%s' returned an error\n", ld->mount_cmd);
                return false;
            }
        }
    }

    if(active_config == botta) {
        if(!construct_botta_launch_cmd(launch_cmd, ld->botta_path, launch_cmd_size)) {
            /* Unmount if necessary */
            if(ld->automatic_mount && mounting_necessary) {
                if(system(ld->umount_cmd) != 0)
                    errorfmt("'%s' returned an error\n", ld->umount_cmd);
            }
            return false;
        }
    }

    sys_format(launch_call, launch_cmd);

    if(system(launch_call) != 0)
        display_error("Failed to launch game\n");

    while(game_running())
        sleep_for(SLEEP_TIME);

    switch(ld->default_state) {
        case rotwk:
            set_active_configuration(rotwk_toml, ld->patch_version, true, false);
            break;
        case edain:
            set_active_configuration(edain_toml, ld->patch_version, ld->swap_dat_file, false);
            break;
        case botta:
            set_active_configuration(botta_toml, ld->patch_version, ld->swap_dat_file, false);
            break;
        default:
            break;
    }

    if(ld->automatic_mount && mounting_necessary) {
        if(system(ld->umount_cmd) != 0) {
            errorfmt("'%s' returned an error\n", ld->umount_cmd);
            return false;
        }
    }

    return true;
}
