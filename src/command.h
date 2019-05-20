#ifndef COMMAND_H
#define COMMAND_H

#include <game_data.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __linux__
#include <unistd.h>
#elif defined __CYGWIN__ || defined _WIN32/* windows */
#include <windows.h>
#endif

#ifdef __linux__
#define PS_LIST "ps cax | grep lotrbfme2wp1.exe > /dev/null 2>&1"
#elif defined __CYGWIN__ || defined _WIN32
#define PS_LIST "PSLIST lotrbfme2ep1.exe > NUL 2>&1"
#else
#error Unknown build environment
#endif

#define SLEEP_TIME 1500

bool game_running(void);
void sleep_for(size_t ms);

void cd_to_game_path(launcher_data const* cfg);

#endif
