#ifndef CLI_INTERFACE
#define CLI_INTERFACE

#ifdef __cplusplus
extern "C" {
#endif

void print_help(void);

int cli_main(int argc, char** argv);

#ifdef __cplusplus
}
#endif

#endif
