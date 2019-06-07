#ifndef GAME_DATA_H
#define GAME_DATA_H

#define ACTIVE_EXT "big"
#define DAT_EXT "dat"
#define SWP_EXT "swp"
#define OTHER_EXT "other"
#define INVALID_EXT "invalid"
#define NEW_DAT_CSUM ""

#include <stdbool.h>

#ifdef __clpusplus
extern "C" {
#endif

typedef enum {
    active,
    inactive
} target_state;

typedef enum {
    rotwk,
    edain,
    botta
} configuration;

typedef struct {
    char name[64];
    char checksum[64];
    char extension[32];
} big_file;

typedef struct {
    char name[64];
    char checksum[64];
    target_state state;
} dat_file;

typedef struct {
    char game_path[128];
    char botta_path[128];
    char mount_exe[256];
    char disc_image[256];
    char mount_flags[32];
    char umount_flags[32];
    char mount_cmd[256];
    char umount_cmd[256];
    bool edain_available;
    bool botta_available;
    bool automatic_mount;
    bool swap_dat_file;
    bool umount_imspec;
} launcher_data;
    
void launcher_data_init(launcher_data* cfg);

#ifdef __clpusplus
}
#endif

#endif
