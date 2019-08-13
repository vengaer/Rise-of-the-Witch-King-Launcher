#ifndef GAME_DATA_H
#define GAME_DATA_H

#define ACTIVE_EXT "big"
#define DAT_EXT "dat"
#define SWP_EXT "swp"
#define OTHER_EXT "other"
#define INVALID_EXT "invalid"
#define NEW_DAT_CSUM "fbff091f6139eb05dc012d834e3eeb74"

#include <stdbool.h>

#define PATH_SIZE 1024
#define LINE_SIZE 256
#define ENTRY_SIZE 64
#define HEADER_SIZE 32
#define VERSION_SIZE 16

#ifdef __cplusplus
extern "C" {
#endif

enum file_state {
    inactive, 
    active
};

enum configuration {
    rotwk = 0x1,
    edain = 0x2,
    botta = 0x4,
    any   = 0x8
};

struct big_file {
    char name[ENTRY_SIZE];
    char checksum[ENTRY_SIZE];
    char extension[HEADER_SIZE];
};

struct dat_file {
    char name[ENTRY_SIZE];
    char checksum[ENTRY_SIZE];
    char disabled[ENTRY_SIZE];
    char introduced[VERSION_SIZE];
    enum file_state state;
};

struct launcher_data {
    char game_path[PATH_SIZE];
    char botta_path[PATH_SIZE];
    char mount_exe[PATH_SIZE];
    char disc_image[PATH_SIZE];
    char mount_flags[HEADER_SIZE];
    char umount_flags[HEADER_SIZE];
    char mount_cmd[PATH_SIZE];
    char umount_cmd[PATH_SIZE];
    char patch_version[HEADER_SIZE];
    bool edain_available;
    bool botta_available;
    bool automatic_mount;
    bool swap_dat_file;
    bool umount_imspec;
    bool kill_on_launch;
    bool show_console;
    bool verify_active;
    enum configuration default_state;
};

void launcher_data_init(struct launcher_data* cfg);

#ifdef __cplusplus
}
#endif

#endif
