#ifndef COMMAND_H
#define COMMAND_H

#include <game_data.h>
#include <stdbool.h>
#include <stddef.h>
#include <unistd.h>

#if defined __CYGWIN__ || defined _WIN32
#include <windows.h>
#endif

#ifdef __linux__
#define PS_LIST "ps cax | grep lotrbfme2wp1.exe > /dev/null 2>&1"
#define SUPPRESS_OUTPUT " > /dev/null"
#elif defined __CYGWIN__ || defined _WIN32
#define PS_LIST "PSLIST lotrbfme2ep1.exe > NUL 2>&1"
#define SUPPRESS_OUTPUT " > NUL"
#else
#error Unknown build environment
#endif

#define SLEEP_TIME 500 /* ms */

#ifdef __cplusplus
extern "C" {
#endif

bool game_running(void);
void sleep_for(size_t ms);

void show_console(bool show);

#ifdef __cplusplus
}
#endif

#endif
