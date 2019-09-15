#ifdef __clang__
#define _POSIX_C_SOURCE 2
#endif

#include "cli.h"
#include "bitop.h"
#include "config.h"
#include "command.h"
#include "crypto.h"
#include "fsys.h"
#include "pattern.h"
#include "progress_bar.h"
#include "progress_callback.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <omp.h>
#include <signal.h>
#include <unistd.h>

extern void(*errdisp)(char const*);
static int volatile cancel_update = 0;

static void print_help(void);
static int get_launcher_dir(char* restrict dst, char const* restrict arg0, size_t dst_size);
static bool setup_config(struct launcher_data* ld, char const* launch_cfg);
static bool construct_rotwkl_toml_path(char* restrict dst, char const* restrict launcher_dir, size_t dst_size);
static bool construct_edain_toml_path(char* restrict dst, char const* restrict launcher_dir, size_t dst_size);
static bool construct_botta_toml_path(char* restrict dst, char const* restrict launcher_dir, size_t dst_size);
static bool construct_launch_cmd(char* restrict dst, char const* restrict game_path, size_t dst_size);
static bool construct_botta_launch_cmd(char* restrict dst, char const* restrict botta_path, size_t dst_size);
static bool construct_dat_file_path(char* restrict dst, char const* restrict game_path, size_t dst_size);
static bool update(char const* restrict upd_cfg, bool new_dat_enabled, struct launcher_data const* ld,
                   char const* restrict edain_toml, char const* restrict botta_toml, char const* restrict rotwk_toml);
static bool update_single_config(enum configuration cfg, char const* toml, bool new_dat_enabled, struct launcher_data const* ld);
static bool update_all_configs(char const* restrict edain_toml, char const* restrict botta_toml, char const* restrict rotwk_toml,
                               bool new_dat_enabled, struct launcher_data const* ld);
static int set(char const* restrict set_cfg, struct launcher_data const* ld, char const* restrict edain_toml,
                char const* restrict botta_toml, char const* restrict rotwk_toml);
static bool launch(char* restrict launch_cmd, size_t launch_cmd_size, char const* restrict dat_file, struct launcher_data const* ld,
                   enum configuration active_config, char const* restrict edain_toml, char const* restrict botta_toml, char const* restrict rotwk_toml);

void signal_handler(int signal) {
    if(signal == SIGINT) {
        atomic_write(&cancel_update, 1);
    }
}

void cli_error_diag(char const* info) {
    fprintf(stderr, "\nError: %s\n\n", info);
}

int cli_main(int argc, char** argv) {
    signal(SIGINT, signal_handler);

    char config_file[PATH_SIZE];
    char launcher_dir[PATH_SIZE];

    bool run_flag = false, set_flag = false, upd_flag = false, h_flag = false;
    char *set_cfg = NULL, *upd_cfg = NULL;
    int idx, opt;
    bool new_dat_enabled;

    char rotwk_toml[PATH_SIZE];
    char edain_toml[PATH_SIZE];
    char botta_toml[PATH_SIZE];
    char launch_cmd[PATH_SIZE];
    char dat_file[PATH_SIZE];
    char game_csum[ENTRY_SIZE];

    struct launcher_data ld;
    char const* launch_cfg = config_file;

    if(get_launcher_dir(launcher_dir, argv[0], sizeof launcher_dir) < 0) {
        errdisp("Could not get launcher directory\n");
        return 1;
    }

    #if defined __GNUC__ && !defined __clang__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wformat-truncation"
    #endif
    snprintf(config_file, sizeof config_file, "%stoml/launcher.toml", launcher_dir);
    #if defined __GNUC__ && !defined __clang__
    #pragma GCC diagnostic pop
    #endif


    while((opt = getopt(argc, argv, ":r:s:u:c:hnv")) != -1) {
        switch(opt) {
            case 'r':
                if(!set_flag && !run_flag) {
                    run_flag = true;
                    set_cfg = optarg;
                }
                break;
            case 's':
                if(!run_flag && !set_flag) {
                    set_flag = true;
                    set_cfg = optarg;
                }
                break;
            case 'u':
                upd_flag = true;
                upd_cfg = optarg;
                break;
            case 'c':
                launch_cfg = optarg;
                break;
            case 'h':
                h_flag = true;
                break;
            case '?':
                if(optopt == 'r' || optopt == 's' || optopt == 'u')
                    errdispf("Option -%c requires an argument\n", optopt);
                else if(isprint(optopt))
                    errdispf("Unknown option '-%c'\n", optopt);
                else
                    errdispf("Unknown option character '\\x%x'\n", optopt);
                return 1;
            default:
                return 1;
        }
    }

    for(idx = optind; idx < argc; idx++)
        errdispf("Non-option argument %s ignored\n", argv[idx]);

    if(h_flag || (!run_flag && !upd_flag && !set_flag)) {
        print_help();
        return 1;
    }

    if(!setup_config(&ld, launch_cfg))
        return 1;

    show_console(ld.show_console);

    if(!construct_rotwkl_toml_path(rotwk_toml, launcher_dir, sizeof rotwk_toml))
        return 1;
    if(!construct_edain_toml_path(edain_toml, launcher_dir, sizeof edain_toml))
        return 1;
    if(!construct_botta_toml_path(botta_toml, launcher_dir, sizeof botta_toml))
        return 1;

    chdir(ld.game_path);

    if(!construct_launch_cmd(launch_cmd, ld.game_path, sizeof launch_cmd))
        return 1;
    if(!construct_dat_file_path(dat_file, ld.game_path, sizeof dat_file))
        return 1;

    md5sum(dat_file, game_csum);
    new_dat_enabled = strcmp(game_csum, NEW_DAT_CSUM) == 0;

    if(upd_cfg && upd_flag) {
        if(!update(upd_cfg, new_dat_enabled, &ld, edain_toml, botta_toml, rotwk_toml))
            return 1;
    }

    enum configuration active_config = ld.default_state;
    if(set_cfg && (set_flag || run_flag)) {
        active_config = set(set_cfg, &ld, edain_toml, botta_toml, rotwk_toml);
        if(active_config < 0)
            return 1;
    }

    if(run_flag) {
        if(!launch(launch_cmd, sizeof launch_cmd, dat_file, &ld, active_config, edain_toml, botta_toml, rotwk_toml))
            return 1;
    }

    return 0;
}

static void print_help(void) {
    fprintf(stderr, "Usage: rotwkl [OPTION]\n\n");
    fprintf(stderr, "    -r <config>,        Run the given configuration.\n");
    fprintf(stderr, "    -s <config>,        Specify what configuration to enable.\n");
    fprintf(stderr, "    -u <config>,        Update config file for specified configuration.\n");
    fprintf(stderr, "    -h,                 Display this help message.\n");
    fprintf(stderr, "Available configurations are:\n");
    fprintf(stderr, "    rotwk\n    edain(*)\n    botta(*)\n    all(**)\n");
    fprintf(stderr, "*  The mod to enable must be installed correctly\n");
    fprintf(stderr, "** Available only with the -u flag\n");
}

static int get_launcher_dir(char* restrict dst, char const* restrict arg0, size_t dst_size) {
    char arg[PATH_SIZE];
    char dir[PATH_SIZE];
    dst[0] = '\0';
    if(is_absolute_path(arg0)) {
        if(strscpy(arg, arg0, sizeof arg) < 0) {
            errdispf("%s overflowed the arg buffer\n", arg0);
            return -E2BIG;
        }
    }
    else{
        if(strscatf(arg, sizeof arg, "%s/%s", dir, arg0) < 0) {
            getcwd(dir, sizeof dir);
            errdispf("%s/%s overflowed the arg buffer\n", dir, arg0);
            return -E2BIG;
        }
    }

    replace_char(arg, '\\', '/');

    parent_path(dir, arg);

    if(strscpy(dst, dir, dst_size) < 0) {
        errdispf("%s overflowed the dst buffer\n", dir);
        return -E2BIG;
    }

    return 0;
}

static bool setup_config(struct launcher_data* ld, char const* launch_cfg) {
    if(!file_exists(launch_cfg)) {
        errdisp("No config file found");
        return false;
    }
    else  {
        if(!read_launcher_config(ld, launch_cfg)) {
            errdisp("Failed to read launcher config");
            return false;
        }
    }
    return true;
}

static bool construct_rotwkl_toml_path(char* restrict dst, char const* restrict launcher_dir, size_t dst_size) {
    if(strscpy(dst, launcher_dir, dst_size) < 0) {
        errdisp("Launcher path overflowed the rotwk toml buffer");
        return false;
    }
    if(strscat(dst, "/toml/rotwk.toml", dst_size) < 0) {
        errdisp("rotwkl toml overflowed the buffer");
        return false;
    }
    return true;
}

static bool construct_edain_toml_path(char* restrict dst, char const* restrict launcher_dir, size_t dst_size) {
    if(strscpy(dst, launcher_dir, dst_size) < 0) {
        errdisp("Launcher path overflowed the edain toml buffer");
        return false;
    }
    if(strscat(dst, "/toml/edain.toml", dst_size) < 0) {
        errdisp("Edain toml overflowed the buffer");
        return false;
    }
    return true;
}

static bool construct_botta_toml_path(char* restrict dst, char const* restrict launcher_dir, size_t dst_size) {
    if(strscpy(dst, launcher_dir, dst_size) < 0) {
        errdisp("Launcher path overflowed the botta toml buffer");
        return false;
    }
    if(strscat(dst, "/toml/botta.toml", dst_size) < 0) {
        errdisp("Botta toml overflowed the buffer");
        return false;
    }
    return true;
}

static bool construct_launch_cmd(char* restrict dst, char const* restrict game_path, size_t dst_size) {
    if(strscpy(dst, game_path, dst_size) < 0) {
        errdisp("Game path overflowed the launch command buffer");
        return false;
    }
    if(strscat(dst, "/lotrbfme2ep1.exe", dst_size) < 0) {
        errdisp("Exe path overflowed the launch command buffer");
        return false;
    }
    return true;
}

static bool construct_botta_launch_cmd(char* restrict dst, char const* restrict botta_path, size_t dst_size) {
    if(strscpy(dst, botta_path, dst_size) < 0) {
        errdisp("Botta path overflowed the launch buffer");
        return false;
    }
    if(strscat(dst, "/BotTa.lnk", dst_size) < 0) {
        errdisp("Botta lnk overflowed the buffer");
        return false;
    }
    return true;
}

static bool construct_dat_file_path(char* restrict dst, char const* restrict game_path, size_t dst_size) {
    if(strscpy(dst, game_path, dst_size) < 0) {
        errdisp("Game path overflowed the game.dat buffer");
        return false;
    }
    if(strscat(dst, "/game.dat", dst_size) < 0) {
        errdisp("game.dat path overflowed the buffer");
        return false;
    }
    return true;
}

static bool update(char const* restrict upd_cfg, bool new_dat_enabled, struct launcher_data const* ld,
                   char const* restrict edain_toml, char const* restrict botta_toml, char const* restrict rotwk_toml) {
    /* Only rotwk */
    if(strcmp(upd_cfg, "rotwk") == 0)
        return update_single_config(rotwk, rotwk_toml, new_dat_enabled, ld);
    /* Only edain */
    else if(strcmp(upd_cfg, "edain") == 0) {

        if(!ld->edain_available) {
            errdisp("Edain is not avaialble");
            return false;
        }

        return update_single_config(edain, edain_toml, new_dat_enabled, ld);
    }
    /* Only botta */
    else if(strcmp(upd_cfg, "botta") == 0) {

        if(!ld->botta_available) {
            errdisp("BotTA is not available");
            return false;
        }

        return update_single_config(botta, botta_toml, new_dat_enabled, ld);
    }

    if(strcmp(upd_cfg, "all") != 0) {
        errdispf("Unknown configuration %s\n", upd_cfg);
        return false;
    }
    /* All */
    return update_all_configs(edain_toml, botta_toml, rotwk_toml, new_dat_enabled, ld);
}

static bool update_single_config(enum configuration cfg, char const* toml, bool new_dat_enabled, struct launcher_data const* ld) {
    bool volatile update_successful;
    int volatile tasks_running = 1;

    atomic_write(&cancel_update, 0);

    struct latch latch;
    latch_init(&latch, tasks_running + 1);

    struct progress_callback pc;
    progress_init(&pc);

    struct progress_bar pb;
    progress_bar_init(&pb);

    bool invert_dat = cfg == rotwk ?
        !new_dat_enabled : new_dat_enabled;

    static char const* cfgs[3] = {"Updating RotWK (Ctrl-C to cancel)", "Updating Edain (Ctrl-C to cancel)", "Updating BotTA (Ctrl-C to cancel)"};

    #pragma omp parallel num_threads(2)
    {
        #pragma omp master
        {

            #pragma omp task
            {
                update_successful = update_game_config(toml, invert_dat, &latch, ld, &pc, &cancel_update);
                atomic_dec(&tasks_running);
            }

            latch_count_down(&latch);
            int tasks = atomic_read(&tasks_running);
            int progress;

            while(tasks) {
                progress = progress_get_percentage(&pc);
                progress_bar_set(&pb, progress);
                progress_bar_display(&pb, cfgs[trailing_zerobits(cfg)]);

                tasks = atomic_read(&tasks_running);
            }
            progress_bar_finish(&pb, cfgs[trailing_zerobits(cfg)]);
        }
    }
    return update_successful;
}
static bool update_all_configs(char const* restrict edain_toml, char const* restrict botta_toml, char const* restrict rotwk_toml,
                               bool new_dat_enabled, struct launcher_data const* ld) {
    int volatile tasks_running = 1;
    if(ld->edain_available)
        ++tasks_running;
    if(ld->botta_available)
        ++tasks_running;

    atomic_write(&cancel_update, 0);

    struct latch latch;
    latch_init(&latch, tasks_running + 1);

    struct progress_callback pc;
    progress_init(&pc);

    struct progress_bar pb;
    progress_bar_init(&pb);

    int volatile failed = 0x0;

    char const desc[] = "Updating all configs (Ctrl-C to cancel)";

    #pragma omp parallel num_threads(4)
    {
        #pragma omp master
        {
            #pragma omp task if(ld->edain_available)
            {
                if(!update_game_config(edain_toml, new_dat_enabled, &latch, ld, &pc, &cancel_update))
                    atomic_or(&failed, edain);

                atomic_dec(&tasks_running);
            }

            #pragma omp task if(ld->botta_available)
            {
                if(!update_game_config(botta_toml, new_dat_enabled, &latch, ld, &pc, &cancel_update))
                    atomic_or(&failed, botta);

                atomic_dec(&tasks_running);
            }

            #pragma omp task
            {
                if(!update_game_config(rotwk_toml, !new_dat_enabled, &latch, ld, &pc, &cancel_update))
                    atomic_or(&failed, rotwk);

                atomic_dec(&tasks_running);
            }

            latch_count_down(&latch);
            int tasks = atomic_read(&tasks_running);
            int progress;

            while(tasks) {
                progress = progress_get_percentage(&pc);
                progress_bar_set(&pb, progress);
                progress_bar_display(&pb, desc);

                tasks = atomic_read(&tasks_running);
            }
            if(!atomic_read(&cancel_update))
                progress_bar_finish(&pb, desc);
        }
    }

    if(failed) {
        if(failed & rotwk)
            errdisp("Failed to update RotWK");
        if(failed & edain)
            errdisp("Failed to update Edain");
        if(failed & botta)
            errdisp("Failed to update BotTA");

        return false;
    }

    return true;
}

static int set(char const* restrict set_cfg, struct launcher_data const* ld, char const* restrict edain_toml,
                char const* restrict botta_toml, char const* restrict rotwk_toml) {
    enum configuration active_config;

    if(strcmp(set_cfg, "rotwk") == 0) {
        set_active_configuration(rotwk_toml, ld->patch_version, true, ld->verify_active);
        active_config = rotwk;
    }
    else {
        if(strcmp(set_cfg, "edain") == 0) {
            if(!ld->edain_available) {
                errdisp("Edain is not available");
                return -1;
            }

            set_active_configuration(edain_toml, ld->patch_version, ld->swap_dat_file, ld->verify_active);

            active_config = edain;
        }
        else if(strcmp(set_cfg, "botta") == 0) {
            if(!ld->botta_available) {
                errdisp("BotTA is not avaialble");
                return -1;
            }

            set_active_configuration(botta_toml, ld->patch_version, ld->swap_dat_file, ld->verify_active);
            active_config = botta;
        }
        else {
            errdisp("Unknown configuration");
            return -1;
        }
    }
    return active_config;
}

static bool launch(char* restrict launch_cmd, size_t launch_cmd_size, char const* restrict dat_file, struct launcher_data const* ld,
                   enum configuration active_config, char const* restrict edain_toml, char const* restrict botta_toml, char const* restrict rotwk_toml) {
    char csum[ENTRY_SIZE];
    char launch_call[PATH_SIZE];
    bool mounting_necessary = true;

    if(ld->automatic_mount) {
        md5sum(dat_file, csum);
        mounting_necessary = strcmp(csum, NEW_DAT_CSUM);

        if(mounting_necessary) {
            if(system(ld->mount_cmd) != 0) {
                errdispf("'%s' returned an error\n", ld->mount_cmd);
                return false;
            }
        }
    }

    if(active_config == botta) {
        if(!construct_botta_launch_cmd(launch_cmd, ld->botta_path, launch_cmd_size)) {
            /* Unmount if necessary */
            if(ld->automatic_mount && mounting_necessary) {
                if(system(ld->umount_cmd) != 0)
                    errdispf("'%s' returned an error\n", ld->umount_cmd);
            }
            return false;
        }
    }

    if(sys_format(launch_call, launch_cmd, sizeof launch_call) < 0) {
        errdisp("Launch command overflowed the system call buffer");
        return false;
    }

    if(system(launch_call) != 0)
        errdisp("Failed to launch game");

    while(game_running())
        sleep_for(SLEEP_TIME);

    switch(ld->default_state) {
        case rotwk:
            set_active_configuration(rotwk_toml, ld->patch_version, true, false);
            break;
        case edain:
            set_active_configuration(edain_toml, ld->patch_version, ld->swap_dat_file, false);
            break;
        case botta:
            set_active_configuration(botta_toml, ld->patch_version, ld->swap_dat_file, false);
            break;
        default:
            break;
    }

    if(ld->automatic_mount && mounting_necessary) {
        if(system(ld->umount_cmd) != 0) {
            errdispf("'%s' returned an error\n", ld->umount_cmd);
            return false;
        }
    }

    return true;
}
