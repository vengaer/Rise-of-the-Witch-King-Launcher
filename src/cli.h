#ifndef CLI_H
#define CLI_H

#include "game_data.h"

#ifdef __cplusplus
extern "C" {
#endif

void print_help(void);

int cli_main(int argc, char** argv);

#ifdef __cplusplus
}
#endif

#endif
