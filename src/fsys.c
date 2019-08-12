#include "fsys.h"
#include "concurrency_utils.h"
#include "config.h"
#include <openssl/md5.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#if defined __CYGWIN__ || _WIN32
#include <windows.h>
#endif

void toggle_big_files(struct big_file* enable, size_t enable_size, struct big_file* disable, size_t disable_size, bool verify_active);
bool handle_swaps(struct dat_file const* swap, size_t swap_size, bool use_version_dat);
void revert_changes(struct big_file* enable, size_t enable_size, struct big_file* disable, size_t disable_size);

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

void set_active_configuration(char const* filename, bool use_version_dat, bool verify_active) {
    size_t enable_size, disable_size, swap_size;
    size_t enable_cap = 64, disable_cap = 64, swap_cap = 4;
    struct big_file* enable = malloc(enable_cap * sizeof(struct big_file));
    struct big_file* disable = malloc(disable_cap * sizeof(struct big_file));
    struct dat_file* swap = malloc(swap_cap * sizeof(struct dat_file));

    read_game_config(filename, &enable, &enable_cap, &enable_size,
                               &disable, &disable_cap, &disable_size,
                               &swap, &swap_cap, &swap_size);


    toggle_big_files(enable, enable_size, disable, disable_size, verify_active);
    bool const swap_successful = handle_swaps(swap, swap_size, use_version_dat);

    if(!swap_successful) {
        SAFE_FPRINTF(stderr, "Failed to swap .dat files, reverting\n");
        revert_changes(enable, enable_size, disable, disable_size);
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
        if(fp) {
            fclose(fp);
            return true;
        }
    #endif

    return false;
}

void game_path_from_registry(char* path) {
    path[0] = '\0';
    #if defined __CYGWIN__ || defined _WIN32
        DWORD dw_size = 1024;
        RegGetValue(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\lotrbfme2ep1.exe", "Path", RRF_RT_ANY, NULL, (PVOID)path, &dw_size);
    #endif
}

void sys_format(char* dst, char const* command) {
    #if defined __CYGWIN__ || defined _WIN32
        sprintf(dst, "\"%s\"", command);
        replace_char(dst, '\'', '\"');
    #else
        strcpy(dst, command);
        replace_char(dst, '\'', ' ');
    #endif
}

void toggle_big_files(struct big_file* enable, size_t enable_size, 
                      struct big_file* disable, size_t disable_size, bool verify_active) {

    #pragma omp parallel 
    {
        size_t i;
        char toggled[FSTR_SIZE];
        char hash[FSTR_SIZE];

        #pragma omp for schedule(static)
        for(i = 0; i < enable_size; i++) {
            strcpy(toggled, enable[i].name);
            set_extension(toggled, enable[i].extension);
            if(file_exists(enable[i].name) && verify_active) {
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
            if(file_exists(toggled) && verify_active) {
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

bool handle_swaps(struct dat_file const* swap, size_t swap_size, bool use_version_dat) {
    size_t i, j;
    char stem[FSTR_SIZE];
    char tmp[FSTR_SIZE];
    char hash[FSTR_SIZE];
    char toggled[FSTR_SIZE];
    struct dat_file const* activate;
    enum file_state target_state;

    bool* done = malloc(swap_size * sizeof(bool));

    for(i = 0; i < swap_size; i++)
        done[i] = false;

    for(i = 0; i < swap_size - 1; i++) {
        if(done[i])
            continue;

        file_stem(stem, swap[i].name);

        /* Find other file in pair */
        for(j = i + 1; j < swap_size; j++) {
            file_stem(tmp, swap[j].name);

            if(strcmp(stem, tmp) == 0)
                break;
        }

        done[i] = done[j] = true;
    

        /* Might not want to swap game.dat */
        if(strcmp(stem, "game") == 0)
            target_state = use_version_dat ? active : inactive;
        else
            target_state = active;

        if(swap[i].state == target_state)
            activate = &swap[i];
        else
            activate = &swap[j];

        md5sum(activate->name, hash);

        /* Already active */
        if(strcmp(activate->checksum, hash) == 0)
            continue;

        strcpy(toggled, activate->name);
        set_extension(toggled, OTHER_EXT);

        md5sum(toggled, hash);

        if(strcmp(activate->checksum, hash) != 0) {
            //TODO: error

            return false;
        }

        strcpy(tmp, activate->name);
        set_extension(tmp, SWP_EXT);
        
        /* .dat -> .swp */
        rename(activate->name, tmp);
        /* .other -> .dat */
        rename(toggled, activate->name);
        /* .swp -> .other */
        rename(tmp, toggled);
    }
    return true;
}

void revert_changes(struct big_file* enable, size_t enable_size, 
                    struct big_file* disable, size_t disable_size) {
    toggle_big_files(disable, disable_size, enable, enable_size, false);

    char const* swp = "game.swp";
    char toggled[FSTR_SIZE];
    strcpy(toggled, swp);
    set_extension(toggled, DAT_EXT);
    if(file_exists(swp))
        rename(swp, toggled);
    else
        SAFE_FPRINTF(stderr, "Failed to restore .dat file\n")
}
