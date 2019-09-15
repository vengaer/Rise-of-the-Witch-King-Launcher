#ifndef CONFIG_H
#define CONFIG_H

#include "game_data.h"
#include "latch.h"
#include "progress_callback.h"
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

bool read_game_config(char const* filename,
                      struct big_file** enable, size_t* enable_capacity, size_t* enable_size,
                      struct big_file** disable, size_t* disable_capacity, size_t* disable_size,
                      struct dat_file** swap, size_t* swap_capacity, size_t* swap_size);

bool write_game_config(char const* filename,
                       struct big_file* enable, size_t enable_size,
                       struct big_file* disable, size_t disable_size,
                       struct dat_file* swap, size_t swap_size);

bool update_game_config(char const* filename, bool invert_dat_files, struct latch* latch, struct launcher_data const* cfg, struct progress_callback* pc, int volatile* cancel);

bool write_launcher_config(struct launcher_data const* cfg, char const* file);
bool read_launcher_config(struct launcher_data* cfg, char const* file);

void construct_mount_command(char* dst, char const* exe, char const* flags, char const* img);
void construct_umount_command(char* dst, char const* exe, char const* flags, char const* img, bool spec_img);

#ifdef __cplusplus
}
#endif

#endif
