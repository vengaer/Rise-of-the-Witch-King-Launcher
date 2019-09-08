#ifndef CLI_H
#define CLI_H

#include "game_data.h"
#include "strutils.h"

#ifdef __cplusplus
extern "C" {
#endif

void print_help(void);

int cli_main(int argc, char** argv);
void cli_error_diag(char const* info);
void cli_error_diagf(char const* fmt, ...);

#ifdef __cplusplus
}
#endif

#endif
