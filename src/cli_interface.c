#include "cli_interface.h"
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
    bool new_dat_enabled, swap;
    
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

    launcher_data ld;
    if(!file_exists(lcfg))
        cli_setup(&ld, lcfg);
    else 
        read_launcher_config(&ld, lcfg);

    show_console(ld.show_console);

    if(!getcwd(cwd, sizeof(cwd))) {
        fprintf(stderr, "Failed to get working directory\n");
        return 1;
    }

    strcpy(rotwk_toml, cwd);
    strcat(rotwk_toml, "/toml/rotwk.toml");

    strcpy(edain_toml, cwd);
    strcat(edain_toml, "/toml/edain.toml");

    strcpy(botta_toml, cwd);
    strcat(botta_toml, "/toml/botta.toml");

    chdir(ld.game_path);

    strcpy(launch_cmd, ld.game_path);
    strcat(launch_cmd, "/lotrbfme2ep1.exe");
    
    strcpy(dat_file, ld.game_path);
    strcat(dat_file, "/game.dat");
    
    md5sum(dat_file, game_csum);
    new_dat_enabled = strcmp(game_csum, NEW_DAT_CSUM) == 0;

    /* Update */
    sync = 1;
    if(u_flag) {
        if(strcmp(ucfg, "rotwk") == 0) 
            update_config_file(rotwk_toml, !new_dat_enabled, &sync, &ld);
        else if(strcmp(ucfg, "edain") == 0) {

            if(!ld.edain_available) {
                fprintf(stderr, "Edain is not available\n");
                return 1;
            }

            update_config_file(edain_toml, new_dat_enabled, &sync, &ld);
        }
        else if(ld.botta_available && strcmp(ucfg, "botta") == 0) {

            if(!ld.botta_available) {
                fprintf(stderr, "BotTA is not available\n");
                return 1;
            }

            update_config_file(botta_toml, new_dat_enabled, &sync, &ld);
        }
        else if(strcmp(ucfg, "all") == 0) {
            if(ld.edain_available)
                ++sync;
            if(ld.botta_available)
                ++sync;
            #pragma omp parallel num_threads(3)
            {
                #pragma omp master
                {
                    prepare_progress();

                    #pragma omp task if(ld.edain_available)
                        update_config_file(edain_toml, new_dat_enabled, &sync, &ld);

                    #pragma omp task if(ld.botta_available)
                        update_config_file(botta_toml, new_dat_enabled, &sync, &ld);
                    
                    update_config_file(rotwk_toml, !new_dat_enabled, &sync, &ld);

                    reset_progress();
                }
            }
        }
        else {
            fprintf(stderr, "Unknown configuration %s\n", ucfg);
            return 1;
        }
    }
    
    configuration active_config;
    
    swap = ld.swap_dat_file ? new_dat_enabled : !new_dat_enabled;
    if(r_flag || s_flag) {
        /* Set active config */
        if(strcmp(scfg, "rotwk") == 0) {
            set_active_configuration(rotwk_toml, true);
            active_config = rotwk;
        }
        else {

            if(strcmp(scfg, "edain") == 0) {
                if(!ld.edain_available) {   
                    fprintf(stderr, "Edain is not available\n");
                    return 1;
                }

                set_active_configuration(edain_toml, swap);

                active_config = edain;
            }
            else if(ld.botta_available && strcmp(scfg, "botta") == 0) {
                if(!ld.botta_available) {
                    fprintf(stderr, "BotTA is not available\n");
                    return 1;
                }

                set_active_configuration(botta_toml, swap);
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
                strcpy(launch_cmd, ld.botta_path);
                strcat(launch_cmd, "/BotTa.lnk");
            }
            
            sys_format(launch, launch_cmd);

            if(system(launch) != 0)
                fprintf(stderr, "Failed to launch game.\n");

            #pragma omp parallel num_threads(2)
            {
                #pragma omp master 
                {
                    #pragma omp task
                    {
                        new_dat_enabled = strcmp(game_csum, NEW_DAT_CSUM) == 0;
                        swap = ld.swap_dat_file ? new_dat_enabled : !new_dat_enabled;
                    }

                    while(game_running())
                        sleep_for(SLEEP_TIME);
                }
            }
            
            switch(ld.default_state) {
                case rotwk:
                    set_active_configuration(rotwk_toml, !new_dat_enabled);
                    break;
                case edain:
                    set_active_configuration(edain_toml, swap);
                    break;
                case botta:
                    set_active_configuration(botta_toml, swap);
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

void cli_setup(launcher_data* cfg, char const* file) {
    launcher_data_init(cfg);
    bool input_ok = false;
    char c;

    char path[PATH_SIZE];

    game_path_from_registry(cfg->game_path);
    printf("Unofficial Rise of the Witch-King Launcher setup\n");
    if(!cfg->game_path[0]) {
        while(!input_ok) {
            printf("Enter the path to the game directory (directory containing lotrbfme2ep1.exe).\n");
            fgets(cfg->game_path, sizeof cfg->game_path, stdin);
            remove_newline(cfg->game_path);

            strcpy(path, cfg->game_path);
            strcat(path, "/lotrbfme2ep1.exe");
            
            if(!file_exists(path)) 
                fprintf(stderr, "Could not locate lotrbfme2ep1.exe at given path. Try again");
            else
                input_ok = true;
        }
    }
    input_ok = false;
    printf("Game path set to '%s'.\n", cfg->game_path);
    printf("Is the Edain mod installed? (y/n)\n");

    while(!input_ok) {
        c = getchar();
        while(getchar() != '\n') { }
        if(c == 'y' || c == 'Y' || c == 'n' || c == 'N')
            input_ok = true;
        else 
            printf("Please enter 'y' or 'n'.\n");
    }
    cfg->edain_available = c == 'y' || c == 'Y';
    input_ok = false;

    printf("Is the Battles of the Third Age (BotTa) mod installed? (y/n)\n");

    while(!input_ok) {
        c = getchar();
        while(getchar() != '\n') { }
        if(c == 'y' || c == 'Y' || c == 'n' || c == 'N')
            input_ok = true;
        else 
            printf("Please enter 'y' or 'n'.\n");
    }
    cfg->botta_available = c == 'y' || c == 'Y';
    input_ok = false;

    if(cfg->botta_available) {
        printf("Enter the path to the BotTa directory.\n");
        while(!input_ok) {
            fgets(cfg->botta_path, sizeof cfg->botta_path, stdin);
            remove_newline(cfg->botta_path);
            
            strcpy(path, cfg->botta_path);
            strcat(path, "/BotTa.lnk");

            if(!file_exists(path))
                fprintf(stderr, "Could not locate BotTa.lnk at given path. Try again.");
            else
                input_ok = true;

        }
        printf("BotTa path set to '%s'.\n", cfg->botta_path);
        input_ok = false;
    }
    else
        cfg->botta_path[0] = '\0';

    printf("Should the launcher mount a disk image automatically? (y/n)\n");
    while(!input_ok) {
        c = getchar();
        while(getchar() != '\n') { }
        if(c == 'y' || c == 'Y' || c == 'n' || c == 'N')
            input_ok = true;
        else 
            printf("Please enter 'y' or 'n'.\n");
    }
    input_ok = false;
    cfg->automatic_mount = c == 'y' || c == 'Y';
    
    if(cfg->automatic_mount) {
        printf("Enter path to mount executable (without quotes or escaping any chars)\n");
        while(!input_ok) {
            fgets(cfg->mount_exe, sizeof cfg->mount_exe, stdin);
            remove_newline(cfg->mount_exe);
            
            if(!file_exists(cfg->mount_exe))
                fprintf(stderr, "%s does not exist\n", cfg->mount_exe);
            else
                input_ok = true;
            
        }
        printf("Mount executable set to '%s'.\n", cfg->mount_exe);
        input_ok = false;

        printf("Enter path to the disc image that should be mounted (without quotes or escaping chars)\n");
        while(!input_ok) {
            fgets(cfg->disc_image, sizeof cfg->disc_image, stdin);
            remove_newline(cfg->disc_image);
            
            if(!file_exists(cfg->disc_image))
                fprintf(stderr, "%s does not exist\n", cfg->disc_image);
            else 
                input_ok = true;
        }
        printf("Disc image set to '%s'.\n", cfg->disc_image);
        input_ok = false;

        printf("Enter mounting flags (if none, leave empty)\n");
        fgets(cfg->mount_flags, sizeof cfg->mount_flags, stdin);
        remove_newline(cfg->mount_flags);
        printf("Mounting flags set to '%s'.\n", cfg->mount_flags);

        printf("Enter unmounting flags (if non, leave empty)\n");
        fgets(cfg->umount_flags, sizeof cfg->umount_flags, stdin);
        remove_newline(cfg->umount_flags);
        printf("Unmounting flags set to '%s'.\n", cfg->umount_flags);

        printf("Should the disc image be specified when invoking the unmoung command?\n");
        while(!input_ok) {
            c = getchar();
            while(getchar() != '\n') { }
            if(c == 'y' || c == 'Y' || c == 'n' || c == 'N')
                input_ok = true;
            else 
                printf("Please enter 'y' or 'n'.\n");
        }
        input_ok = false;
        cfg->umount_imspec = c == 'y' || c == 'Y';

        construct_mount_command(cfg->mount_cmd, cfg->mount_exe, cfg->mount_flags, cfg->disc_image);
        construct_umount_command(cfg->umount_cmd, cfg->mount_exe, cfg->umount_flags, cfg->disc_image, cfg->umount_imspec);
        printf("Mount command set ot '%s'.\n", cfg->mount_cmd);
        printf("Unmount command set ot '%s'.\n", cfg->umount_cmd);
    }

    printf("Should the launcher swap dat files? (Recommended).");

    while(!input_ok) {
        c = getchar();
        while(getchar() != '\n') { }
        if(c == 'y' || c == 'Y' || c == 'n' || c == 'N')
            input_ok = true;
        else 
            printf("Please enter 'y' or 'n'.\n");
    }
    cfg->swap_dat_file = c == 'y' || c == 'Y';

    write_launcher_config(cfg, file);
}
