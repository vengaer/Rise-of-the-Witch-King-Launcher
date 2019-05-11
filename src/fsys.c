#include "fsys.h"
#include "config.h"
#include "game_files.h"

#include <openssl/md5.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __linux__
#include <unistd.h>
#elif defined _WIN32
#include <tchar.h>
#include <atlstr.h>
#endif

void set_extension(char* filename, char const* extension);
void rename_files(big_file* enable, size_t enable_size, big_file* disable, size_t disable_size);
bool file_exists(char const* filename);
void revert_changes(big_file* enable, size_t enable_size, big_file* disable, size_t disable_size);

bool md5sum(char const* filename, char* checksum) {
    int i, num_bytes;
    FILE* file = fopen(filename, "rb");
    
    if(file == NULL) {
        fprintf(stderr, "%s could not be opened for hashing\n", filename);
        return false;
    }

    char data[CHUNK_SIZE];
    unsigned char hash[MD5_DIGEST_LENGTH];
    MD5_CTX context;
    MD5_Init(&context);

    while((num_bytes = fread(data, 1, CHUNK_SIZE, file)))
        MD5_Update(&context, data, num_bytes);

    MD5_Final(hash, &context);
    fclose(file);

    for(i = 0; i < MD5_DIGEST_LENGTH; i++)
        sprintf(&checksum[i*2], "%02x", (unsigned int)hash[i]);

    return true;
}

void update_config_file(char const* filename) {
    size_t enable_size, disable_size, swap_size;
    size_t enable_cap = 64, disable_cap = 64, swap_cap = 2;
    big_file* enable = malloc(enable_cap * sizeof(big_file));
    big_file* disable = malloc(disable_cap * sizeof(big_file));
    dat_file* swap = malloc(swap_cap * sizeof(dat_file));
    
    read_game_config(filename, &enable, &enable_cap, &enable_size,
                               &disable, &disable_cap, &disable_size,
                               &swap, &swap_cap, &swap_size);
    
    size_t i;
    for(i = 0; i < enable_size; i++)
        if(!md5sum(enable[i].name, enable[i].checksum))
            return;
    for(i = 0; i < disable_size; i++) 
        if(!md5sum(disable[i].name, disable[i].checksum))
            return;
    for(i = 0; i < swap_size; i++)
        if(!md5sum(swap[i].name, swap[i].checksum))
            return;

    write_game_config(filename, enable, enable_size, 
                                disable, disable_size, 
                                swap, swap_size);

    free(enable);
    free(disable);
    free(swap);
}

void set_configuration(char const* filename) {
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
    rename_files(enable, enable_size, disable, disable_size);

    for(i = 0; i < swap_size; i++) {
        if(swap[i].state == active) {
            md5sum(swap[i].name, hash);
            if(file_exists(swap[i].name) && strcmp(swap[i].checksum, hash) == 0)
                return;
        }
    }

    // omp
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
    strcpy(++ext_begin, extension);
}

bool file_exists(char const* filename) {
#ifdef __linux__
    if(access(filename, F_OK) != -1)
        return true;
    return false;

#elif defined _WIN32
    TCHAR tname[128];
    _tcscpy(tname, A2T(filename));
    
    WIN32_FIND_DATA find_file_data;
    HANDLE handle = FindFirstFile(tname, &find_file_data);
    
    bool found = handle != INVALID_HANDLE_VALUE;
    if(found)
        FindClose(handle);

    return found;
#endif

    return false;
}

void rename_files(big_file* enable, size_t enable_size, big_file* disable, size_t disable_size) {
    size_t i;
    char toggled[64];
    char hash[64];

    // omp
    for(i = 0; i < enable_size; i++) {
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
                if(file_exists(enable[i].name))
                    remove(enable[i].name);
                continue;
            }
        }
        strcpy(toggled, enable[i].name);
        set_extension(toggled, enable[i].extension);
        rename(toggled, enable[i].name);
    }
    for(i = 0; i < disable_size; i++) {
        if(file_exists(disable[i].name)) {
            md5sum(disable[i].name, hash);
            if(strcmp(disable[i].checksum, hash) != 0) {
                char invalid[64];
                strcpy(invalid, disable[i].name);
                set_extension(invalid, INVALID_EXT);
                fprintf(stderr, "Warning: File %s already exists. Will be moved to %s\n", disable[i].name, invalid);

                rename(disable[i].name, invalid);
            }
            else {
                if(file_exists(disable[i].name))
                    remove(disable[i].name);
                continue;
            }
        }

        strcpy(toggled, disable[i].name);
        set_extension(toggled, disable[i].extension);
        rename(disable[i].name, toggled);
    }

}

void revert_changes(big_file* enable, size_t enable_size, big_file* disable, size_t disable_size) {
    rename_files(disable, disable_size, enable, enable_size);
    
    char const* swp = "game.swp";
    char toggled[64];
    strcpy(toggled, swp);
    set_extension(toggled, DAT_EXT);
    if(file_exists(swp))
        rename(swp, toggled);
    else
        fprintf(stderr, "Failed to restore .dat file\n");
}

