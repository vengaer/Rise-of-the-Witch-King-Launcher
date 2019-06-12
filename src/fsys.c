#include "fsys.h"
#include "config.h"
#include "thread_lock.h"

#include <openssl/md5.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#if defined __CYGWIN__ || _WIN32
#include <windows.h>
#endif

void toggle_big_files(big_file* enable, size_t enable_size, big_file* disable, size_t disable_size);
void revert_changes(big_file* enable, size_t enable_size, big_file* disable, size_t disable_size);

bool md5sum(char const* filename, char* checksum) {
    int i, num_bytes;
    FILE* fp = fopen(filename, "rb");
    
    if(!fp) {
        SAFE_FPRINTF(stderr, "%s could not be opened for hashing\n", filename)
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

void set_active_configuration(char const* filename, bool use_version_dat) {
    size_t i;
    size_t enable_size, disable_size, swap_size;
    size_t enable_cap = 64, disable_cap = 64, swap_cap = 2;
    big_file* enable = malloc(enable_cap * sizeof(big_file));
    big_file* disable = malloc(disable_cap * sizeof(big_file));
    dat_file* swap = malloc(swap_cap * sizeof(dat_file));
    
    read_game_config(filename, &enable, &enable_cap, &enable_size,
                               &disable, &disable_cap, &disable_size,
                               &swap, &swap_cap, &swap_size);

    char toggled[FSTR_SIZE];
    char hash[FSTR_SIZE];

    toggle_big_files(enable, enable_size, disable, disable_size);
    
    file_state target_state = use_version_dat ? active : inactive;

    for(i = 0; i < swap_size; i++) {
        if(swap[i].state == target_state) {
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
        if(swap[i].state != target_state) {
            md5sum(swap[i].name, hash);
            if(strcmp(swap[i].checksum, hash) != 0) {
                SAFE_FPRINTF(stderr, "Checksum for %s is incorrect. Reverting changes\n", swap[i].name)
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
        if(swap[i].state == target_state) {
            strcpy(toggled, swap[i].name);
            set_extension(toggled, OTHER_EXT);
            md5sum(toggled, hash);
            if(strcmp(swap[i].checksum, hash) != 0) {
                SAFE_FPRINTF(stderr, "Checksum for %s is incorrect. Reverting changes\n", swap[i].name)
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
        if(swap[i].state != target_state) {
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

void sys_format(char* syscall, char const* orig_command) {
    #if defined __CYGWIN__ || defined _WIN32
        syscall[0] = '\"';
        strcpy(syscall + 1, orig_command);
        replace_char(syscall, '\'', '\"');
        int len = strlen(syscall);
        syscall[len] = '\"';
        syscall[len + 1] = '\0';
    #else
        strcpy(syscall, orig_command);
        replace_char(syscall, '\'', ' ');
    #endif
}

void toggle_big_files(big_file* enable, size_t enable_size, big_file* disable, size_t disable_size) {

    #pragma omp parallel 
    {
        size_t i;
        char toggled[FSTR_SIZE];
        char hash[FSTR_SIZE];

        #pragma omp for schedule(static)
        for(i = 0; i < enable_size; i++) {
            strcpy(toggled, enable[i].name);
            set_extension(toggled, enable[i].extension);
            if(file_exists(enable[i].name)) {
                md5sum(enable[i].name, hash);
                if(strcmp(enable[i].checksum, hash) != 0) {
                    char invalid[FSTR_SIZE];
                    strcpy(invalid, enable[i].name);
                    set_extension(invalid, INVALID_EXT);
                    SAFE_FPRINTF(stderr, "Warning: File %s already exists. Will be moved to %s\n", enable[i].name, invalid)

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

        #pragma omp for schedule(static)
        for(i = 0; i < disable_size; i++) {
            strcpy(toggled, disable[i].name);
            set_extension(toggled, disable[i].extension);
            if(file_exists(toggled)) {
                md5sum(toggled, hash);
                if(strcmp(disable[i].checksum, hash) != 0) {
                    char invalid[FSTR_SIZE];
                    strcpy(invalid, toggled);
                    set_extension(invalid, INVALID_EXT);
                    SAFE_FPRINTF(stderr, "Warning: File %s already exists. Will be moved to %s\n", toggled, invalid)

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
    char toggled[FSTR_SIZE];
    strcpy(toggled, swp);
    set_extension(toggled, DAT_EXT);
    if(file_exists(swp))
        rename(swp, toggled);
    else
        SAFE_FPRINTF(stderr, "Failed to restore .dat file\n")
}

