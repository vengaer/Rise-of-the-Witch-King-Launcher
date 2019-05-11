#include "config.h"
#include "fsys.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char** argv) {
    if(argc < 2) {
        fprintf(stderr, "Missing options. Try rotwkl --help\n");
        return 1;
    }
    
    if(argv[1][0] != '-') {
        fprintf(stderr, "First argument must be one or more options. Try rotwkl --help\n");
        return 1;
    }
        
    if(strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
        fprintf(stderr, "Usage: rotwkl [OPTION] [VERSION]\n\n");
        fprintf(stderr, "    -e, --enable        Specify what configuration to enable\n");
        fprintf(stderr, "    -u, --update        Update config file for specified configuration\n");
        fprintf(stderr, "    -h, --help          Display this help message\n");
        fprintf(stderr, "Available configurations are:\n");
        fprintf(stderr, "    rotwk\n    edain(*)\n    botta(*)\n");
        fprintf(stderr, "* The mod to enable must be installed correctly\n");

        return 1;
    }

    if(strcmp(argv[1], "--enable") == 0 || strcmp(argv[1], "-e") == 0) {
        if(argc < 3) {
            fprintf(stderr, "%s must be followed by a configuration\n", argv[1]);
            return 1;
        }
        if(strcmp(argv[2], "rotwk") == 0) {
            set_configuration("toml/rotwk.toml");
        }
        else if(strcmp(argv[2], "edain") == 0) {
            set_configuration("toml/edain.toml");
        }
        else if(strcmp(argv[2], "botta") == 0) {
            set_configuration("toml/botta.toml");
        }
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



    /*
    char str[50];
    unsigned char hash[MD5_DIGEST_LENGTH];
    if(!md5sum("toml/edain.toml", hash))
        return 0;
    int j;
    for(j = 0; j < MD5_DIGEST_LENGTH; j++)
        sprintf(&str[j * 2], "%02x", (unsigned int)hash[j]);
    printf("%s\n", str);
    */
    /*
    big_file* bf = malloc(100 * sizeof(big_file));
    size_t bf_cap = 100;
    big_file* df = malloc(100 * sizeof(big_file));
    size_t df_cap = 100;
    size_t swp_cap = 4;
    size_t bf_size, df_size, swp_size;
    dat_file* swp = malloc(swp_cap * sizeof(dat_file));
    read_game_config("toml/edain.toml", &bf, &bf_cap, &bf_size, &df, &df_cap, &df_size, &swp, &swp_cap, &swp_size);
    
    int i;
    printf("Enable\n");
    for(i = 0; i < bf_size; i++) {
        printf("%s\n", bf[i].name);
        printf("%s\n", bf[i].checksum);
        printf("%s\n\n", bf[i].extension);
    }
    printf("Disable\n");
    for(i = 0; i < df_size; i++) {
        printf("%s\n", df[i].name);
        printf("%s\n", df[i].checksum);
        printf("%s\n\n", df[i].extension);
    }
    printf("Swap:\n");
    for(i = 0; i < swp_size; i++) {
        printf("%s\n", swp[i].name);
        printf("%s\n", swp[i].checksum);
        if(swp[i].state == active)
            printf("active\n");
        else
            printf("!active\n");
    }
    

    free(bf);
    free(df);
    free(swp);
    */

    return 0;
}
