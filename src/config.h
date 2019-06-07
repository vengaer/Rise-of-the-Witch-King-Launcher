#ifndef CONFIG_H
#define CONFIG_H

#include "game_data.h"
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void read_game_config(char const* filename, 
                      big_file** enable, 
                      size_t* enable_capacity, 
                      size_t* enable_size,
                      big_file** disable, 
                      size_t* disable_capacity, 
                      size_t* disable_size,
                      dat_file** swap, 
                      size_t* swap_capacity,
                      size_t* swap_size);

void write_game_config(char const* filename, 
                       big_file* enable, 
                       size_t enable_size, 
                       big_file* disable,
                       size_t disable_size,
                       dat_file* swap,
                       size_t swap_size);

void cli_setup(launcher_data* cfg, char const* file);

void write_launcher_config(launcher_data const* cfg, char const* file);
bool read_launcher_config(launcher_data* cfg, char const* file);

void construct_mount_command(char* dst, char const* exe, char const* flags, char const* img);

void construct_umount_command(char* dst, char const* exe, char const* flags, char const* img, bool spec_img);

void construct_from_rel_path(launcher_data const* cfg, char* dst, char const* rel_path);

static char const CONFIG_FILE[] = "launcher.toml";

#ifdef __cplusplus
}
#endif

#endif
