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
#define FSTR_SIZE 64
#define HEADER_SIZE 32
#define OPT_SIZE 32

#ifdef __clpusplus
extern "C" {
#endif

typedef enum {
    inactive, 
    active
} file_state;

typedef enum {
    rotwk = 0x1,
    edain = 0x2,
    botta = 0x4,
    any   = 0x8
} configuration;

typedef struct {
    char name[FSTR_SIZE];
    char checksum[FSTR_SIZE];
    char extension[OPT_SIZE];
} big_file;

typedef struct {
    char name[FSTR_SIZE];
    char checksum[FSTR_SIZE];
    file_state state;
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
    bool show_console;
    configuration default_state;
} launcher_data;
    
void launcher_data_init(launcher_data* cfg);

#ifdef __clpusplus
}
#endif

#endif
