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
#define FSTR_SIZE 64
#define OPT_SIZE 32

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
    char name[FSTR_SIZE];
    char checksum[FSTR_SIZE];
    char extension[OPT_SIZE];
} big_file;

typedef struct {
    char name[FSTR_SIZE];
    char checksum[FSTR_SIZE];
    target_state state;
} dat_file;

typedef struct {
    char game_path[PATH_SIZE];
    char botta_path[PATH_SIZE];
    char mount_exe[PATH_SIZE];
    char disc_image[PATH_SIZE];
    char mount_flags[OPT_SIZE];
    char umount_flags[OPT_SIZE];
    char mount_cmd[PATH_SIZE];
    char umount_cmd[PATH_SIZE];
    bool edain_available;
    bool botta_available;
    bool automatic_mount;
    bool swap_dat_file;
    bool umount_imspec;
    bool kill_on_launch;
} launcher_data;
    
void launcher_data_init(launcher_data* cfg);

#ifdef __clpusplus
}
#endif

#endif
