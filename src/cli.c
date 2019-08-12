#include "cli.h"
#include "config.h"
#include "command.h"
#include "fsys.h"
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

int cli_main(int argc, char** argv) {
    static char const config_file[] = "toml/launcher.toml";
    char const* lcfg = config_file;

    bool r_flag = false, s_flag = false, u_flag = false, h_flag = false;
    char *scfg = NULL, *ucfg = NULL;
    int idx, opt, sync;
    bool mounting_necessary = true; 
    bool new_dat_enabled;
    struct latch latch;

    char cwd[PATH_SIZE];
    char rotwk_toml[PATH_SIZE];
    char edain_toml[PATH_SIZE];
    char botta_toml[PATH_SIZE];
    char launch_cmd[PATH_SIZE];
    char launch[PATH_SIZE];
    char dat_file[PATH_SIZE];
    char game_csum[FSTR_SIZE];

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

    struct launcher_data ld;
    if(!file_exists(lcfg)) {
        fprintf(stderr, "Warning: no config file found. Using default values\n");
        launcher_data_init(&ld);
    }
    else 
        read_launcher_config(&ld, lcfg);

    show_console(ld.show_console);

    if(!getcwd(cwd, sizeof(cwd))) {
        fprintf(stderr, "Failed to get working directory\n");
        return 1;
    }

    strncpy(rotwk_toml, cwd, sizeof rotwk_toml);
    strncat(rotwk_toml, "/toml/rotwk.toml", sizeof rotwk_toml - strlen(cwd));

    strncpy(edain_toml, cwd, sizeof edain_toml - 1);
    strncat(edain_toml, "/toml/edain.toml", sizeof edain_toml - strlen(cwd) - 1);

    strncpy(botta_toml, cwd, sizeof botta_toml - 1);
    strncat(botta_toml, "/toml/botta.toml", sizeof botta_toml - strlen(cwd) - 1);

    chdir(ld.game_path);

    strncpy(launch_cmd, ld.game_path, sizeof launch_cmd);
    strncat(launch_cmd, "/lotrbfme2ep1.exe", sizeof launch_cmd - strlen(ld.game_path));

    strncpy(dat_file, ld.game_path, sizeof dat_file - 1);
    strncat(dat_file, "/game.dat", sizeof dat_file - strlen(ld.game_path) - 1);

    md5sum(dat_file, game_csum);
    new_dat_enabled = strcmp(game_csum, NEW_DAT_CSUM) == 0;

    /* Update */
    sync = 1;
    if(strcmp(ucfg, "all") == 0) {
        if(ld.edain_available)
            ++sync;
        if(ld.botta_available)
            ++sync;
    }
    latch_init(&latch, sync);

    if(u_flag) {
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

    if(r_flag || s_flag) {
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
                strncpy(launch_cmd, ld.botta_path, sizeof launch_cmd);
                strncat(launch_cmd, "/BotTa.lnk", sizeof launch_cmd - strlen(ld.botta_path));
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

