#ifndef FSYS_H
#define FSYS_H

#include <stdbool.h>

#define CHUNK_SIZE 1024

bool md5sum(char const* filename, char* csum);

void update_config_file(char const* filename);

void set_active_configuration(char const* filename, bool should_swap);

void set_extension(char* filename, char const* extension);

bool file_exists(char const* filename);

#endif
