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

void cd_to_game_path(launcher_data const* cfg) {
    char game_path[128];
    game_path[0] = '\"';
    strcpy(game_path + 1, cfg->game_path);
    strcat(game_path, "\"");
    chdir(game_path);
}
