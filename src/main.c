#include "config.h"
#include "command.h"
#include "fsys.h"
#include "gui.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char** argv) {
    if(argc < 2) {
        if(!file_exists(CONFIG_FILE)) {
            /* gui setup */
        }
        gui_run(argc, argv);
        return 0;
    }
    
    if(argv[1][0] != '-') {
        fprintf(stderr, "First argument must be one or more options. Try rotwkl --help\n");
        return 1;
    }
        
    if(strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
        fprintf(stderr, "Usage: rotwkl [OPTION] [VERSION]\n\n");
        fprintf(stderr, "    -r, --run           Run the given configuration\n");
        fprintf(stderr, "    -s, --set           Specify what configuration to enable\n");
        fprintf(stderr, "    -u, --update        Update config file for specified configuration\n");
        fprintf(stderr, "    -h, --help          Display this help message\n");
        fprintf(stderr, "Available configurations are:\n");
        fprintf(stderr, "    rotwk\n    edain(*)\n    botta(*)\n");
        fprintf(stderr, "* The mod to enable must be installed correctly\n");

        return 1;
    }
    if(strcmp(argv[1], "--run") == 0 || strcmp(argv[1], "-r") == 0) {
        if(argc < 3) {
            fprintf(stderr, "%s must be followed by a configuration\n", argv[1]);
            return 1;
        }
        if(strcmp(argv[2], "rotwk") == 0)
            active_configuration("toml/rotwk.toml");
        else if(strcmp(argv[2], "edain") == 0) 
            active_configuration("toml/edain.toml");
        else if(strcmp(argv[2], "botta") == 0) 
            active_configuration("toml/botta.toml");
        else {
            fprintf(stderr, "Unknown configuration %s\n", argv[2]);
            return 1;
        }
        /* mount */
        launch_game();

        while(game_running())
            sleep_for(SLEEP_TIME);
        
        /* umount */

    }
    else if(strcmp(argv[1], "--set") == 0 || strcmp(argv[1], "-s") == 0) {
        if(argc < 3) {
            fprintf(stderr, "%s must be followed by a configuration\n", argv[1]);
            return 1;
        }
        if(strcmp(argv[2], "rotwk") == 0) 
            active_configuration("toml/rotwk.toml");
        else if(strcmp(argv[2], "edain") == 0) 
            active_configuration("toml/edain.toml");
        else if(strcmp(argv[2], "botta") == 0) 
            active_configuration("toml/botta.toml");
        else {
            fprintf(stderr, "Unknown configuration %s\n", argv[2]);
            return 1;
        }
    }
    else if(strcmp(argv[1], "--update") == 0 || strcmp(argv[1], "-u") == 0) {
        if(argc < 3) {
            fprintf(stderr, "%s must be followed by a configuration\n", argv[1]);
            return 1;
        }
        if(strcmp(argv[2], "rotwk") == 0) {
            update_config_file("toml/rotwk.toml");
        }
        else if(strcmp(argv[2], "edain") == 0) {
            update_config_file("toml/edain.toml");
        }
        else if(strcmp(argv[2], "botta") == 0) {
            update_config_file("toml/botta.toml");
        }
        else {
            fprintf(stderr, "Unknown configuration %s\n", argv[2]);
            return 1;
        }
    }
    else {
        fprintf(stderr, "Missing options. Try rotwkl --help\n");
        return 1;
    }


    return 0;
}
