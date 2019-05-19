#ifndef GAME_DATA_H
#define GAME_DATA_H

#define ACTIVE_EXT "big"
#define DAT_EXT "dat"
#define SWP_EXT "swp"
#define OTHER_EXT "other"
#define INVALID_EXT "invalid"

#include <stdbool.h>

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
    char mount_cmd[256];
    char umount_cmd[256];
    bool edain_available;
    bool botta_available;
    bool automatic_mount;
} launcher_data;
    

#endif