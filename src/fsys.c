#include "fsys.h"
#include "command.h"
#include "config.h"
#include "game_data.h"

#include <openssl/md5.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#if defined __CYGWIN__ || _WIN32
#include <windows.h>
#endif

int progress = 0, total_work = -1;

void toggle_big_files(big_file* enable, size_t enable_size, big_file* disable, size_t disable_size);
void revert_changes(big_file* enable, size_t enable_size, big_file* disable, size_t disable_size);

bool md5sum(char const* filename, char* checksum) {
    int i, num_bytes;
    FILE* fp = fopen(filename, "rb");
    
    if(!fp) {
        fprintf(stderr, "%s could not be opened for hashing\n", filename);
        return false;
    }

    char data[CHUNK_SIZE];
    unsigned char hash[MD5_DIGEST_LENGTH];
    MD5_CTX context;
    MD5_Init(&context);

    while((num_bytes = fread(data, 1, CHUNK_SIZE, fp)))
        MD5_Update(&context, data, num_bytes);

    MD5_Final(hash, &context);
    fclose(fp);

    for(i = 0; i < MD5_DIGEST_LENGTH; i++)
        sprintf(&checksum[i*2], "%02x", (unsigned int)hash[i]);

    return true;
}

void prepare_progress(void) {
    progress = 0;
    total_work = 0;
}

void reset_progress(void) {
    progress = 0;
    total_work = -1;
}

double track_progress(void) {
    return (double)progress / (double)total_work;
}

void update_config_file(char const* filename, bool invert_dat_files) {
    size_t enable_size, disable_size, swap_size;
    size_t enable_cap = 64, disable_cap = 64, swap_cap = 2;
    big_file* enable = malloc(enable_cap * sizeof(big_file));
    big_file* disable = malloc(disable_cap * sizeof(big_file));
    dat_file* swap = malloc(swap_cap * sizeof(dat_file));
    
    read_game_config(filename, &enable, &enable_cap, &enable_size,
                               &disable, &disable_cap, &disable_size,
                               &swap, &swap_cap, &swap_size);
    #pragma omp atomic
    total_work += enable_size + disable_size + swap_size;

    bool success = true;

    #pragma omp parallel 
    {
        size_t i;
        char toggled[64];
        bool config_enabled;

        #pragma omp for reduction(&& : success)
        for(i = 0; i < enable_size; i++) {
            if(file_exists(enable[i].name)) {
                if(!md5sum(enable[i].name, enable[i].checksum))
                    success = false;
            }
            else {
                config_enabled = false;
                strcpy(toggled, enable[i].name);
                set_extension(toggled, enable[i].extension);
                if(!md5sum(toggled, enable[i].checksum))
                   success = false;
            }
            #pragma omp atomic
            ++progress;
        }
        #pragma omp for reduction(&& : success)
        for(i = 0; i < disable_size; i++) {
            if(file_exists(disable[i].name)) {
                if(!md5sum(disable[i].name, disable[i].checksum))
                    success = false;
            }
            else {
                config_enabled = true;
                strcpy(toggled, disable[i].name);
                set_extension(toggled, disable[i].extension);
                if(!md5sum(toggled, disable[i].checksum))
                    success = false;
            }
            #pragma omp atomic
            ++progress;
        }
        #pragma omp for reduction(&& : success)
        for(i = 0; i < swap_size; i++) {
            strcpy(toggled, swap[i].name);
            set_extension(toggled, OTHER_EXT);

            if(swap[i].state == active) {
                if(config_enabled && !invert_dat_files) {
                    if(!md5sum(swap[i].name, swap[i].checksum))
                        success = false;
                }
                else {
                    if(!md5sum(toggled, swap[i].checksum))
                        success = false;
                }
            }
            else {
                if(config_enabled && !invert_dat_files) {
                    if(!md5sum(toggled, swap[i].checksum))
                        success = false;
                }
                else {
                    if(!md5sum(swap[i].name, swap[i].checksum))
                        success = false;
                }
            }
            #pragma omp atomic
            ++progress;
        }

    }

    if(success) {
        write_game_config(filename, enable, enable_size, 
                                    disable, disable_size, 
                                    swap, swap_size);
    }
    else 
        fprintf(stderr, "Errors were encountered during hashing, config file will remain unchanged\n");

    free(enable);
    free(disable);
    free(swap);
}



void set_active_configuration(char const* filename, bool should_swap) {
    size_t i;
    size_t enable_size, disable_size, swap_size;
    size_t enable_cap = 64, disable_cap = 64, swap_cap = 2;
    big_file* enable = malloc(enable_cap * sizeof(big_file));
    big_file* disable = malloc(disable_cap * sizeof(big_file));
    dat_file* swap = malloc(swap_cap * sizeof(dat_file));
    
    read_game_config(filename, &enable, &enable_cap, &enable_size,
                               &disable, &disable_cap, &disable_size,
                               &swap, &swap_cap, &swap_size);

    char toggled[64];
    char hash[64];

    toggle_big_files(enable, enable_size, disable, disable_size);

    if(!should_swap) {
        free(enable);
        free(disable);
        free(swap);
        return;
    }

    for(i = 0; i < swap_size; i++) {
        if(swap[i].state == active) {
            md5sum(swap[i].name, hash);
            if(file_exists(swap[i].name) && strcmp(swap[i].checksum, hash) == 0) {
                free(enable);
                free(disable);
                free(swap);
                return;
            }
        }
    }

    for(i = 0; i < swap_size; i++) {
        if(swap[i].state == inactive) {
            md5sum(swap[i].name, hash);
            if(strcmp(swap[i].checksum, hash) != 0) {
                fprintf(stderr, "Checksum for %s is incorrect. Reverting changes\n", swap[i].name);
                revert_changes(enable, enable_size, disable, disable_size);
                free(enable);
                free(disable);
                free(swap);
                return;
            }
            strcpy(toggled, swap[i].name);
            set_extension(toggled, SWP_EXT);
            rename(swap[i].name, toggled);
            strcpy(swap[i].name, toggled);
        }
    }
    for(i = 0; i < swap_size; i++) {
        if(swap[i].state == active) {
            strcpy(toggled, swap[i].name);
            set_extension(toggled, OTHER_EXT);
            md5sum(toggled, hash);
            if(strcmp(swap[i].checksum, hash) != 0) {
                fprintf(stderr, "Checksum for %s is incorrect. Reverting changes\n", swap[i].name);
                revert_changes(enable, enable_size, disable, disable_size);
                free(enable);
                free(disable);
                free(swap);
                return;
            }
            rename(toggled, swap[i].name);
        }
    }
    for(i = 0; i < swap_size; i++) {
        if(swap[i].state == inactive) {
            strcpy(toggled, swap[i].name);
            set_extension(toggled, OTHER_EXT);
            rename(swap[i].name, toggled);
        }
    }

    free(enable);
    free(disable);
    free(swap);
}

void set_extension(char* filename, char const* extension) {
    char* ext_begin = strrchr(filename, '.');

    if(strcmp(ext_begin, ".bak") == 0) {
        filename[strlen(filename) - strlen(ext_begin)] = '\0';
        ext_begin = strrchr(filename, '.');
    }

    strcpy(++ext_begin, extension);
}

bool file_exists(char const* filename) {
#ifdef __linux__
    return access(filename, F_OK) != -1;
#else
    FILE* fp = fopen(filename, "r");
    bool exists = fp != NULL;
    if(exists)
        fclose(fp);

    return exists;
#endif

    return false;
}

void game_path_from_registry(char* path) {
    path[0] = '\0';
#if !defined __CYGWIN__ && !defined(_WIN32)
    return;
#else
    DWORD dw_size = 1024;
    RegGetValue(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\lotrbfme2ep1.exe", "Path", RRF_RT_ANY, NULL, (PVOID)path, &dw_size);
#endif
}

void toggle_big_files(big_file* enable, size_t enable_size, big_file* disable, size_t disable_size) {

    #pragma omp parallel 
    {
        size_t i;
        char toggled[64];
        char hash[64];

        #pragma omp for
        for(i = 0; i < enable_size; i++) {
            strcpy(toggled, enable[i].name);
            set_extension(toggled, enable[i].extension);
            if(file_exists(enable[i].name)) {
                md5sum(enable[i].name, hash);
                if(strcmp(enable[i].checksum, hash) != 0) {
                    char invalid[64];
                    strcpy(invalid, enable[i].name);
                    set_extension(invalid, INVALID_EXT);
                    fprintf(stderr, "Warning: File %s already exists. Will be moved to %s\n", enable[i].name, invalid);

                    rename(enable[i].name, invalid);
                }
                else {
                    if(file_exists(toggled))
                        remove(toggled);
                    continue;
                }
            }
            
            rename(toggled, enable[i].name);
        }

        #pragma omp for
        for(i = 0; i < disable_size; i++) {
            strcpy(toggled, disable[i].name);
            set_extension(toggled, disable[i].extension);
            if(file_exists(toggled)) {
                md5sum(toggled, hash);
                if(strcmp(disable[i].checksum, hash) != 0) {
                    char invalid[64];
                    strcpy(invalid, toggled);
                    set_extension(invalid, INVALID_EXT);
                    fprintf(stderr, "Warning: File %s already exists. Will be moved to %s\n", toggled, invalid);

                    rename(toggled, invalid);
                }
                else {
                    if(file_exists(disable[i].name))
                        remove(disable[i].name);
                    continue;
                }
            }

            rename(disable[i].name, toggled);
        }
    }

}

void revert_changes(big_file* enable, size_t enable_size, big_file* disable, size_t disable_size) {
    toggle_big_files(disable, disable_size, enable, enable_size);
    
    char const* swp = "game.swp";
    char toggled[64];
    strcpy(toggled, swp);
    set_extension(toggled, DAT_EXT);
    if(file_exists(swp))
        rename(swp, toggled);
    else
        fprintf(stderr, "Failed to restore .dat file\n");
}

