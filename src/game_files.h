#ifndef GAME_FILES_H
#define GAME_FILES_H

#define ACTIVE_EXT "big"
#define DAT_EXT "dat"
#define SWP_EXT "swp"
#define OTHER_EXT "other"
#define INVALID_EXT "invalid"

typedef enum {
    active,
    inactive
} target_state;

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

#endif
