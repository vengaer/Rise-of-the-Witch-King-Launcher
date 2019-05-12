#ifndef CONFIG_H
#define CONFIG_H

#define CONFIG_FILE "rotwkl.toml"

#include "game_files.h"
#include <stddef.h>

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
                       

#endif
