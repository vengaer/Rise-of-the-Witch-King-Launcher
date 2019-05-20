#include "config.h"
#include "command.h"
#include "glade_gui.h"
#include "fsys.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* TODO: fix botta */
/* TODO: handle paths (when run from different dir) */
/* Add csum to game_data.h, check in glade_gui before mounting, fix launch cmd if config not found  */
/* Popups, tooltips, resizing, sensitivity umount_imspec, disable buttons if mods not installed */

static void print_help(void) {
    fprintf(stderr, "Usage: rotwkl [OPTION]\n\n");
    fprintf(stderr, "    -r <config>,        Run the given configuration.\n");
    fprintf(stderr, "    -s <config>,        Specify what configuration to enable.\n");
    fprintf(stderr, "    -u <config>,        Update config file for specified configuration.\n");
    fprintf(stderr, "    -c <path>,          Specify path to launcher config file.\n");
    fprintf(stderr, "    -n                  Do not swap .dat files.\n");
    fprintf(stderr, "    -h,                 Display this help message.\n");
    fprintf(stderr, "Available configurations are:\n");
    fprintf(stderr, "    rotwk\n    edain(*)\n    botta(*)\n");
    fprintf(stderr, "* The mod to enable must be installed correctly\n");
}

int main(int argc, char** argv) {
    static char const CONFIG_FILE[] = "rotwkl.toml";

    if(argc < 2) {
        gui_init(&argc, &argv);
        //print_help();
        return 0;
    }

    char const* lcfg = CONFIG_FILE;

    bool r_flag = false, s_flag = false, u_flag = false, h_flag = false, n_flag = false;
    char *scfg = NULL, *ucfg = NULL;
    int idx, opt;

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
            case 'n':
                n_flag = true;
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
                abort();
        }
    }

    for(idx = optind; idx < argc; idx++)
        printf("Non-option argument %s ignored.\n", argv[idx]);
    
    if(h_flag) {
        print_help();
        return 1;
    }

    launcher_data ld;
    if(!file_exists(lcfg))
        cli_setup(&ld, lcfg);
    else 
        read_launcher_config(&ld, lcfg);

    cd_to_game_path(&ld);

    char launch_cmd[256];
    char rotwk_toml[128];
    char edain_toml[128];
    char botta_toml[128];
    
    construct_from_rel_path(&ld, launch_cmd, "/lotrbfme2ep1.exe");
    construct_from_rel_path(&ld, rotwk_toml, "/toml/rotwk.toml");
    construct_from_rel_path(&ld, edain_toml, "/toml/edain.toml");
    construct_from_rel_path(&ld, botta_toml, "/toml/botta.toml");

    if(u_flag) {
        if(strcmp(ucfg, "rotwk") == 0)
            update_config_file(rotwk_toml);
        else if(ld.edain_available && strcmp(ucfg, "edain") == 0)
            update_config_file(edain_toml);
        else if(ld.botta_available && strcmp(ucfg, "botta") == 0)
            update_config_file(botta_toml);
        else {
            fprintf(stderr, "Unknown configuration %s\n", ucfg);
            return 1;
        }
    }
    
    configuration active_config;
    
    if(r_flag || s_flag) {
        if(strcmp(scfg, "rotwk") == 0) {
            set_active_configuration(rotwk_toml, !n_flag);
            active_config = rotwk;
        }
        else if(ld.edain_available && strcmp(scfg, "edain") == 0) {
            set_active_configuration(edain_toml, !n_flag);
            active_config = edain;
        }
        else if(ld.botta_available && strcmp(scfg, "botta") == 0) {
            set_active_configuration(botta_toml, !n_flag);
            active_config = botta;
        }
        else {
            fprintf(stderr, "Unknown configuration %s\n", scfg);
            return 1;
        }
        if(r_flag) {
            char game_csum[64];
            md5sum("game.dat", game_csum);
            bool mounting_necessary = strcmp(game_csum, NEW_DAT_CSUM);
            
            if(mounting_necessary && ld.automatic_mount) {
                if(system(ld.mount_cmd) != 0) {
                    fprintf(stderr, "'%s' returned an error\n", ld.mount_cmd);
                    return 1;
                }
            }
        
            if(active_config == botta) {
                strcat(launch_cmd, " -mod ");
                strcat(launch_cmd, ld.botta_path);
            }

            if(system(launch_cmd) != 0)
                fprintf(stderr, "Failed to launch game.\n");

            while(game_running())
                sleep_for(SLEEP_TIME);
            
            if(mounting_necessary && ld.automatic_mount) {
                if(system(ld.umount_cmd) != 0) {
                    fprintf(stderr, "'%s' returned an error\n", ld.umount_cmd);
                    return 1;
                }
            }
        }
    }
    


    return 0;
}
