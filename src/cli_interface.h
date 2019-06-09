#ifndef CLI_INTERFACE
#define CLI_INTERFACE

#include "game_data.h"

#ifdef __cplusplus
extern "C" {
#endif

void print_help(void);

int cli_main(int argc, char** argv);
void cli_setup(launcher_data* cfg, char const* file);

#ifdef __cplusplus
}
#endif

#endif
