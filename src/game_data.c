#include "game_data.h"
#include <stdio.h>

void launcher_data_init(struct launcher_data* cfg) {
    cfg->game_path[0]     = '\0';
    cfg->botta_path[0]    = '\0';
    cfg->mount_exe[0]     = '\0';
    cfg->disc_image[0]    = '\0';
    cfg->mount_flags[0]   = '\0';
    cfg->umount_flags[0]  = '\0';
    cfg->mount_cmd[0]     = '\0';
    cfg->umount_cmd[0]    = '\0';
    cfg->edain_available  = false;
    cfg->botta_available  = false;
    cfg->automatic_mount  = false;
    cfg->swap_dat_file    = true;
    cfg->umount_imspec    = false;
    cfg->kill_on_launch   = true;
    cfg->show_console     = false;
    cfg->verify_active    = true;
    cfg->default_state    = rotwk;
    // todo check if available
    sprintf(cfg->patch_version, "v8.0.0");
}
