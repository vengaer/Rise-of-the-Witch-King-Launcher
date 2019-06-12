#ifndef FSYS_H
#define FSYS_H

#include <stdbool.h>
#include "command.h"
#include "game_data.h"

#define CHUNK_SIZE 1024

#ifdef __cplusplus
extern "C" {
#endif

bool md5sum(char const* filename, char* csum);

void set_active_configuration(char const* filename, bool use_version_dat);

void set_extension(char* filename, char const* extension);

bool file_exists(char const* filename);

void game_path_from_registry(char* path);

void sys_format(char* syscall, char const* orig_command);

#ifdef __cplusplus
}
#endif

#endif
