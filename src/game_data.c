#include "game_data.h"
#include <unistd.h>

void launcher_data_new(launcher_data* cfg) {
    getcwd(cfg->game_path, sizeof(cfg->game_path));
    cfg->botta_path[0] = '\0';
    cfg->mount_cmd[0] = '\0';
    cfg->umount_cmd[0] = '\0';
    cfg->edain_available = false;
    cfg->botta_available = false;
    cfg->automatic_mount = false;
    cfg->swap_dat_file = true;
}
