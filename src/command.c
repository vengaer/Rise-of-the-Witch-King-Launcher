#include "command.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool game_running(void) {
    return system(PS_LIST) == 0;
}

void sleep_for(size_t ms) {
#ifdef __linux__
    sleep(ms / 1000);
#elif defined __CYGWIN__ || defined _WIN32
    Sleep(ms);
#endif
}

void show_console(bool show) {
    #if defined __CYGWIN__ || defined _WIN32
    if(show)
        ShowWindow(GetConsoleWindow(), SW_SHOW);
    else
        ShowWindow(GetConsoleWindow(), SW_HIDE);
    #endif
}
