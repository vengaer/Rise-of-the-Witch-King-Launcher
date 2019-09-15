#include "fsys.h"
#include "config.h"
#include "crypto.h"
#include "pattern.h"
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#if defined __CYGWIN__ || _WIN32
#include <windows.h>
#endif

extern void(*errdisp)(char const*);

static bool toggle_big_files(struct big_file* enable, size_t enable_size, struct big_file* disable, size_t disable_size, char const* target_version, bool verify_active);
static bool enable_big_file(struct big_file const* file, bool verify_active);
static bool disable_big_file(struct big_file const* file, bool verify_active);
static bool handle_swaps(struct dat_file const* swap, size_t swap_size, char const* target_version, bool use_version_dat);
static void revert_changes(struct big_file* enable, size_t enable_size, struct big_file* disable, size_t disable_size, char const* target_version);

void set_active_configuration(char const* restrict filename, char const* restrict target_version,
                              bool use_version_dat, bool verify_active) {
    size_t enable_size, disable_size, swap_size;
    size_t enable_cap = 64, disable_cap = 64, swap_cap = 4;
    struct big_file* enable = malloc(enable_cap * sizeof(struct big_file));
    struct big_file* disable = malloc(disable_cap * sizeof(struct big_file));
    struct dat_file* swap = malloc(swap_cap * sizeof(struct dat_file));
    bool read_success, toggle_success, swap_success;

    read_success = read_game_config(filename, &enable, &enable_cap, &enable_size,
                                              &disable, &disable_cap, &disable_size,
                                              &swap, &swap_cap, &swap_size);



    if(read_success) {
        toggle_success = toggle_big_files(enable, enable_size, disable, disable_size, target_version, verify_active);

        if(!toggle_success) {
            errdisp("Failed to toggle .big files, reverting");
            revert_changes(enable, enable_size, disable, disable_size, target_version);
        }
        else {
            swap_success = handle_swaps(swap, swap_size, target_version, use_version_dat);

            if(!swap_success) {
                errdisp("Failed to swap .dat files, reverting");
                revert_changes(enable, enable_size, disable, disable_size, target_version);
            }
        }
    }
    else
        errdisp("Errors encountered while reading game config, no changes will be made\n");

    free(enable);
    free(disable);
    free(swap);
}

int set_extension(char* restrict filename, char const* restrict extension, size_t count) {
    size_t stem_len;
    char* ext_begin = strrchr(filename, '.');

    /* For .big.bak files (edain) */
    if(strcmp(ext_begin, ".bak") == 0) {
        filename[strlen(filename) - strlen(ext_begin)] = '\0';
        ext_begin = strrchr(filename, '.');
    }
    stem_len = ++ext_begin - filename;
    strncpy(ext_begin, extension, count - stem_len);

    /* Not null-terminated, overflow */
    if(filename[count - 1]) {
        filename[count - 1] = '\0';
        return -E2BIG;
    }

    return stem_len + strlen(extension);
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

void file_stem(char* dst, char const* file) {
    char* end = strchr(file, '.');
    memcpy(dst, file, end - file);
    dst[end - file] = '\0';
}

int parent_path(char* dst, char const* file, size_t count) {
    char* end = strrchr(file, '/') + 1;
    size_t size, distance;

    if(end) {
        distance = end - file + 1;
        size = distance < count ? distance : count;
        memcpy(dst, file, size);
        dst[size - 1] = '\0';

        /* Overflow */
        if(distance > count)
            return -E2BIG;

        return strlen(dst);
    }

    return strscpy(dst, file, count);
}

static bool toggle_big_files(struct big_file* enable, size_t enable_size,
                      struct big_file* disable, size_t disable_size,
                      char const* target_version, bool verify_active) {

    bool toggle_successful = true;
    #pragma omp parallel reduction(&& : toggle_successful)
    {
        size_t i;
        char in_ver[HEADER_SIZE];

        #pragma omp for schedule(static)
        for(i = 0; i < enable_size; i++) {
            if(!toggle_successful)
                continue;

            version_introduced_in(in_ver, enable[i].name);

            /* Introduced no later than target version */
            if(!in_ver[0] || strcmp(in_ver, target_version) <= 0) {
                if(!enable_big_file(&enable[i], verify_active)) {
                    toggle_successful = false;
                }
            }
            /* Introduced after target version */
            else {
                if(!disable_big_file(&enable[i], verify_active)) {
                    toggle_successful = false;
                }
            }
        }

        #pragma omp for schedule(static)
        for(i = 0; i < disable_size; i++) {
            if(!toggle_successful)
                continue;

            if(!disable_big_file(&disable[i], verify_active)) {
                toggle_successful = false;
            }
        }
    }
    return toggle_successful;
}

static bool enable_big_file(struct big_file const* file, bool verify_active) {
    char toggled[MEMBER_SIZE(struct big_file, name)];
    char hash[ENTRY_SIZE];

    strcpy(toggled, file->name);
    if(set_extension(toggled, file->extension, sizeof toggled) < 0) {
        errdispf("Appending extension %s to %s would overflow the buffer", file->extension, toggled);
        return false;
    }

    if(file_exists(file->name) && verify_active) {
        md5sum(file->name, hash);
        if(strcmp(file->checksum, hash) != 0) {
            char invalid[MEMBER_SIZE(struct big_file, name)];
            strcpy(invalid, file->name);
            if(set_extension(invalid, INVALID_EXT, sizeof invalid) < 0) {
                errdispf("Appending extension %s to %s would overflow the buffer", INVALID_EXT, invalid);
                return false;
            }
            errdispf("File %s already exists, will be moved to %s", file->name, invalid);

            rename(file->name, invalid);
        }
        else {
            if(file_exists(toggled))
                remove(toggled);
            return true;
        }
    }

    rename(toggled, file->name);
    return true;
}

static bool disable_big_file(struct big_file const* file, bool verify_active) {
    char toggled[MEMBER_SIZE(struct big_file, name)];
    char hash[ENTRY_SIZE];

    strcpy(toggled, file->name);
    if(set_extension(toggled, file->extension, sizeof toggled) < 0) {
        errdispf("Appending extension %s to %s would overflow the buffer", file->extension, toggled);
        return false;
    }

    if(file_exists(toggled) && verify_active) {
        md5sum(toggled, hash);
        if(strcmp(file->checksum, hash) != 0) {
            char invalid[MEMBER_SIZE(struct big_file, name)];
            strcpy(invalid, toggled);
            if(set_extension(invalid, INVALID_EXT, sizeof invalid) < 0) {
                errdispf("Appending extension %s to %s would overflow the buffer", INVALID_EXT, invalid);
                return false;
            }
            errdispf("File %s already exists, will be moved to %s\n", toggled, invalid);

            rename(toggled, invalid);
        }
        else {
            if(file_exists(file->name))
                remove(file->name);
            return true;
        }
    }

    rename(file->name, toggled);
    return true;
}

static bool handle_swaps(struct dat_file const* swap, size_t swap_size, char const* target_version, bool use_version_dat) {
    size_t i, j;
    char stem[ENTRY_SIZE];
    char tmp[MEMBER_SIZE(struct dat_file, name)];
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
            errdisp("No file matches the desired checksum");
            free(done);
            return false;
        }

        strcpy(tmp, activate->name);
        if(set_extension(tmp, SWP_EXT, sizeof tmp) < 0) {
            errdispf("Appending extension %s to %s would overflow the buffer", SWP_EXT, tmp);
            return false;
        }

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

static void revert_changes(struct big_file* enable, size_t enable_size,
                    struct big_file* disable, size_t disable_size,
                    char const* target_version) {
    toggle_big_files(disable, disable_size, enable, enable_size, target_version, false);

    char const* swp = "game.swp";
    char toggled[ENTRY_SIZE];
    strcpy(toggled, swp);
    set_extension(toggled, DAT_EXT, sizeof toggled);
    if(file_exists(swp))
        rename(swp, toggled);
    else
        errdisp("Failed to restore .dat file\n");
}
