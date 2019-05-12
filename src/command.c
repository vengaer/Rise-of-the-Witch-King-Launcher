#include "command.h"

#include <stdio.h>
#include <stdlib.h>

bool game_running() {
    return system(PS_LIST) == 0;
}

void launch_game() {
    if(system(LAUNCH_COMMAND) != 0)
        fprintf(stderr, "Failed to launch game\n");
}

void sleep_for(size_t ms) {
#ifdef __linux__
    sleep(ms / 1000);
#elif defined __CYGWIN__ || defined _WIN32
    Sleep(ms);
#endif
}
