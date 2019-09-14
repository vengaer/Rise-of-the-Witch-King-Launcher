#ifndef FSYS_H
#define FSYS_H

#include <stdbool.h>
#include "command.h"
#include "game_data.h"
#include "strutils.h"

#ifdef __cplusplus
extern "C" {
#endif

void set_active_configuration(char const* RESTRICT filename, char const* RESTRICT target_version, bool use_version_dat, bool verify_active);
void set_extension(char* RESTRICT filename, char const* RESTRICT extension);
bool file_exists(char const* filename);
void game_path_from_registry(char* path);
void file_stem(char* dst, char const* file);
void parent_path(char* dst, char const* file);

#ifdef __cplusplus
}
#endif

#endif
