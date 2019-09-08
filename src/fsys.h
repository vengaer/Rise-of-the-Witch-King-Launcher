#ifndef FSYS_H
#define FSYS_H

#include "strutils.h"
#include <stdbool.h>
#include "command.h"
#include "game_data.h"

#define CHUNK_SIZE 1024

#ifdef __cplusplus
extern "C" {
#endif

bool md5sum(char const* RESTRICT filename, char* RESTRICT csum);
void set_active_configuration(char const* RESTRICT filename, char const* RESTRICT target_version, bool use_version_dat, bool verify_active);
void set_extension(char* RESTRICT filename, char const* RESTRICT extension);
bool file_exists(char const* filename);
void game_path_from_registry(char* path);

#ifdef __cplusplus
}
#endif

#endif
