#include "fsys.h"
#include "config.h"
#include "pattern.h"
#include <openssl/md5.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#if defined __CYGWIN__ || _WIN32
#include <windows.h>
#endif

extern void(*display_error)(char const*);

void toggle_big_files(struct big_file* enable, size_t enable_size, struct big_file* disable, size_t disable_size, char const* target_version, bool verify_active);
void enable_big_file(struct big_file const* file, bool verify_active);
void disable_big_file(struct big_file const* file, bool verify_active);
bool handle_swaps(struct dat_file const* swap, size_t swap_size, char const* target_version, bool use_version_dat);
void revert_changes(struct big_file* enable, size_t enable_size, struct big_file* disable, size_t disable_size, char const* target_version);

bool md5sum(char const* restrict filename, char* restrict checksum) {
    int i, num_bytes;
    FILE* fp = fopen(filename, "rb");

    if(!fp) {
        errorfmt("%s could not be opened for hashing\n", filename);
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

void set_active_configuration(char const* restrict filename, char const* restrict target_version,
                              bool use_version_dat, bool verify_active) {
    size_t enable_size, disable_size, swap_size;
    size_t enable_cap = 64, disable_cap = 64, swap_cap = 4;
    struct big_file* enable = malloc(enable_cap * sizeof(struct big_file));
    struct big_file* disable = malloc(disable_cap * sizeof(struct big_file));
    struct dat_file* swap = malloc(swap_cap * sizeof(struct dat_file));

    bool const read_success = read_game_config(filename, &enable, &enable_cap, &enable_size,
                                                         &disable, &disable_cap, &disable_size,
                                                         &swap, &swap_cap, &swap_size);



    if(read_success) {
        toggle_big_files(enable, enable_size, disable, disable_size, target_version, verify_active);
        bool const swap_successful = handle_swaps(swap, swap_size, target_version, use_version_dat);

        if(!swap_successful) {
            display_error("Failed to swap .dat files, reverting\n");
            revert_changes(enable, enable_size, disable, disable_size, target_version);
        }
    }
    else
        display_error("Errors encountered while reading game config, no changes will be made\n");

    free(enable);
    free(disable);
    free(swap);
}

void set_extension(char* restrict filename, char const* restrict extension) {
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

void toggle_big_files(struct big_file* enable, size_t enable_size,
                      struct big_file* disable, size_t disable_size,
                      char const* target_version, bool verify_active) {

    #pragma omp parallel
    {
        size_t i;
        char in_ver[HEADER_SIZE];

        #pragma omp for schedule(static)
        for(i = 0; i < enable_size; i++) {
            version_introduced_in(in_ver, enable[i].name);

            /* Introduced no later than target version */
            if(!in_ver[0] || strcmp(in_ver, target_version) <= 0)
                enable_big_file(&enable[i], verify_active);
            /* Introduced after target version */
            else
                disable_big_file(&enable[i], verify_active);
        }

        #pragma omp for schedule(static)
        for(i = 0; i < disable_size; i++)
            disable_big_file(&disable[i], verify_active);
    }
}

void enable_big_file(struct big_file const* file, bool verify_active) {
    char toggled[ENTRY_SIZE];
    char hash[ENTRY_SIZE];

    strcpy(toggled, file->name);
    set_extension(toggled, file->extension);

    if(file_exists(file->name) && verify_active) {
        md5sum(file->name, hash);
        if(strcmp(file->checksum, hash) != 0) {
            char invalid[ENTRY_SIZE];
            strcpy(invalid, file->name);
            set_extension(invalid, INVALID_EXT);
            errorfmt("File %s already exists, will be moved to %s\n", file->name, invalid);

            rename(file->name, invalid);
        }
        else {
            if(file_exists(toggled))
                remove(toggled);
            return;
        }
    }

    rename(toggled, file->name);
}

void disable_big_file(struct big_file const* file, bool verify_active) {
    char toggled[ENTRY_SIZE];
    char hash[ENTRY_SIZE];

    strcpy(toggled, file->name);
    set_extension(toggled, file->extension);

    if(file_exists(toggled) && verify_active) {
        md5sum(toggled, hash);
        if(strcmp(file->checksum, hash) != 0) {
            char invalid[ENTRY_SIZE];
            strcpy(invalid, toggled);
            set_extension(invalid, INVALID_EXT);
            errorfmt("File %s already exists, will be moved to %s\n", toggled, invalid);

            rename(toggled, invalid);
        }
        else {
            if(file_exists(file->name))
                remove(file->name);
            return;
        }
    }

    rename(file->name, toggled);
}

bool handle_swaps(struct dat_file const* swap, size_t swap_size, char const* target_version, bool use_version_dat) {
    size_t i, j;
    char stem[ENTRY_SIZE];
    char tmp[ENTRY_SIZE];
    char hash[ENTRY_SIZE];
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

        if(swap[i].state == target_state && strcmp(swap[i].introduced, target_version) <= 0)
            activate = &swap[i];
        else
            activate = &swap[j];

        /* File introduced later than target version */
        if(strcmp(activate->introduced, target_version) > 0)
            continue;

        md5sum(activate->name, hash);

        /* Already active */
        if(strcmp(activate->checksum, hash) == 0)
            continue;

        md5sum(activate->disabled, hash);

        if(strcmp(activate->checksum, hash) != 0) {
            display_error("No file matches the desired checksum\n");
            free(done);
            return false;
        }

        strcpy(tmp, activate->name);
        set_extension(tmp, SWP_EXT);
 
        /* .dat -> .swp */
        rename(activate->name, tmp);
        /* .other -> .dat */
        rename(activate->disabled, activate->name);
        /* .swp -> .other */
        rename(tmp, activate->disabled);
    }

    free(done);
    return true;
}

void revert_changes(struct big_file* enable, size_t enable_size,
                    struct big_file* disable, size_t disable_size,
                    char const* target_version) {
    toggle_big_files(disable, disable_size, enable, enable_size, target_version, false);

    char const* swp = "game.swp";
    char toggled[ENTRY_SIZE];
    strcpy(toggled, swp);
    set_extension(toggled, DAT_EXT);
    if(file_exists(swp))
        rename(swp, toggled);
    else
        display_error("Failed to restore .dat file\n");
}
