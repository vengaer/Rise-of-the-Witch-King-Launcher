#ifndef COMMAND_H
#define COMMAND_H

#include <stdbool.h>

#ifdef __linux__
#include <unistd.h>
#elif defined _WIN32
#include <windows.h>
#else
#error "Unknown OS"
#endif

#ifdef __linux__
#define PS_LIST "ps cax | grep lotrbfme2wp1.exe > /dev/null 2>&1"
#define LAUNCH_COMMAND "./lotrbfme2ep1.exe"
#elif defined _WIN32
#define PS_LIST "PSLIST lotrbfme2ep1.exe > NUL 2>&1"
#define LAUNCH_COMMAND "lotrbfme2ep1.exe"
#endif

#define SLEEP_TIME 1500

bool game_running();
void launch_game();
void sleep_for(size_t ms);

#endif
