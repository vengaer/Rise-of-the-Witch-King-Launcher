#include "config.h"
#include "atomic.h"
#include "bitop.h"
#include "crypto.h"
#include "fsys.h"
#include "pattern.h"
#include "strutils.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern void(*errdisp)(char const*);

static void header_name(char* line, char* header);
static void subheader_name(char* line, char* header);
static void get_table_key(char* entry, char* key);
static void get_table_value(char const* entry, char* value);
static bool read_big_entry(char* line, struct big_file* entry);
static bool read_dat_entry(char* line, struct dat_file* entry);

bool read_game_config(char const* filename,
                      struct big_file** enable, size_t* enable_capacity, size_t* enable_size,
                      struct big_file** disable, size_t* disable_capacity, size_t* disable_size,
                      struct dat_file** swap, size_t* swap_capacity, size_t* swap_size) {

    char line[LINE_SIZE];
    char header[HEADER_SIZE];
    char subheader[HEADER_SIZE];

    enum line_contents contents;
    unsigned line_number = 0;

    *enable_size = 0;
    *disable_size = 0;
    *swap_size = 0;

    FILE* fp = fopen(filename, "r");
    if(!fp) {
        errdispf("%s could not be opened\n", filename);
        return false;
    }
    else {
        fseek(fp, 0, SEEK_END);
        size_t file_size = ftell(fp);
        if(file_size == 0) {
            errdispf("%S is empty\n", filename);
            fclose(fp);
            return false;
        }
    }

    rewind(fp);
    while(fgets(line, sizeof line, fp)) {
        ++line_number;
        contents = determine_line_contents(line);

        if(contents == content_invalid) {
            errdispf("Syntax error on line %u in %s: %s\n", line_number, filename, line);
            fclose(fp);
            return false;
        }
        else if(contents == content_blank)
            continue;
        else if(contents == content_subheader) {
            subheader_name(line, subheader);
            if(strcmp(subheader, "enable") == 0)
                ++(*enable_size);
            else if(strcmp(subheader, "disable") == 0)
                ++(*disable_size);
            else if(strcmp(subheader, "swap") != 0) {
                errdispf("Syntax error on line %u in %s: %s\nUnknown subheader\n", line_number, filename, line);
                fclose(fp);
                return false;
            }

            continue;
        }
        else {
            if(strcmp(subheader, "enable") == 0) {
                if(*enable_size >= *enable_capacity) {
                    *enable = realloc(*enable, 2 * (*enable_capacity) * sizeof(struct big_file));
                    *enable_capacity *= 2;
                    if(!*enable) {
                        errdisp("Failed to reallocate enable");
                        fclose(fp);
                        return false;
                    }
                }

                if(!read_big_entry(line, &(*enable)[(*enable_size) - 1])) {
                    errdispf("Syntax error on line %u in %s: %s\n", line_number, filename, line);
                    fclose(fp);
                    return false;
                }
            }
            else if(strcmp(subheader, "disable") == 0) {
                if(*disable_size >= *disable_capacity) {
                    *disable = realloc(*disable, 2 * (*disable_capacity) * sizeof(struct big_file));
                    *disable_capacity *= 2;
                    if(!*disable) {
                        errdisp("Failed to reallocate disable");
                        fclose(fp);
                        return false;
                    }
                }

                if(!read_big_entry(line, &(*disable)[(*disable_size) - 1])) {
                    errdispf("Syntax error on line %u in %s: %s\n", line_number, filename, line);
                    fclose(fp);
                    return false;
                }

            }
            else if(strcmp(subheader, "swap") == 0) {
                if(contents == content_header) {
                    header_name(line, header);
                    if(strcmp(header, "swap.activate") == 0)
                        (*swap)[(*swap_size)].state = active;
                    else
                        (*swap)[(*swap_size)].state = inactive;
                    ++(*swap_size);
                    continue;
                }
                if(*swap_size >= *swap_capacity) {
                    *swap = realloc(*swap, 2 * (*swap_capacity) * sizeof(struct dat_file));
                    *swap_capacity *= 2;
                    if(!*swap) {
                        errdisp("Failed to reallocate swap");
                        fclose(fp);
                        return false;
                    }
                }
                if(!read_dat_entry(line, &(*swap)[(*swap_size) - 1])) {
                    errdispf("Invalid entry '%s' in %s\n", line, filename);
                    fclose(fp);
                    return false;
                }
            }
            else {
                errdispf("Unknown header %s\n", header);
                fclose(fp);
                return false;
            }
        }
    }

    fclose(fp);
    return true;
}

bool write_game_config(char const* filename,
                       struct big_file* enable, size_t enable_size,
                       struct big_file* disable, size_t disable_size,
                       struct dat_file* swap, size_t swap_size) {

    FILE* fp = fopen(filename, "w");
    if(!fp) {
        errdispf("%s could not be opened\n", filename);
        return false;
    }
    size_t i;
    for(i = 0; i < enable_size; i++) {
        fprintf(fp, "[[enable]]\n");
        fprintf(fp, "name = \"%s\"\n", enable[i].name);
        fprintf(fp, "checksum = \"%s\"\n", enable[i].checksum);
        fprintf(fp, "extension = \"%s\"\n\n", enable[i].extension);
    }
    for(i = 0; i < disable_size; i++) {
        fprintf(fp, "[[disable]]\n");
        fprintf(fp, "name = \"%s\"\n", disable[i].name);
        fprintf(fp, "checksum = \"%s\"\n", disable[i].checksum);
        fprintf(fp, "extension = \"%s\"\n\n", disable[i].extension);
    }
    fprintf(fp, "[[swap]]\n");
    for(i = 0; i < swap_size; i++) {
        if(swap[i].state == active)
            fprintf(fp, "    [swap.activate]\n");
        else
            fprintf(fp, "    [swap.deactivate]\n");
        fprintf(fp, "        name = \"%s\"\n", swap[i].name);
        fprintf(fp, "        checksum = \"%s\"\n", swap[i].checksum);
        fprintf(fp, "        disabled = \"%s\"\n", swap[i].disabled);
        fprintf(fp, "        introduced = \"%s\"\n", swap[i].introduced);
    }

    fclose(fp);
    return true;
}

bool update_game_config(char const* filename, bool invert_dat_files, struct latch* latch, struct launcher_data const* cfg, struct progress_callback* pc, int volatile* cancel) {
    bool read_success, update_success, write_success;
    size_t enable_size, disable_size, swap_size;
    size_t enable_cap = 64, disable_cap = 64, swap_cap = 2;
    struct big_file* enable = malloc(enable_cap * sizeof(struct big_file));
    struct big_file* disable = malloc(disable_cap * sizeof(struct big_file));
    struct dat_file* swap = malloc(swap_cap * sizeof(struct dat_file));

    if(!enable || !disable || !swap) {
        errdisp("Failed to allocate memory");
        free(enable);
        free(disable);
        free(swap);
        return false;
    }

    read_success = read_game_config(filename, &enable, &enable_cap, &enable_size,
                                              &disable, &disable_cap, &disable_size,
                                              &swap, &swap_cap, &swap_size);

    if(!read_success) {
        errdisp("Failed to read game config");
        free(enable);
        free(disable);
        free(swap);
        return false;
    }

    progress_add_total(pc, enable_size + disable_size + swap_size);
    latch_count_down(latch);

    update_success = true;

    #pragma omp parallel reduction(&& : update_success)
    {
        size_t i;
        char toggled[MEMBER_SIZE(struct big_file, name)];
        bool config_enabled;
        bool is_canceled = false;

        #pragma omp for schedule(dynamic)
        for(i = 0; i < enable_size; i++) {
            if(is_canceled || (!cfg->edain_available && strstr(enable[i].name, "edain")))
                continue;

            if(!file_exists(enable[i].name)) {
                config_enabled = false;
                strcpy(toggled, enable[i].name);
                if(set_extension(toggled, enable[i].extension, sizeof toggled) < 0) {
                    errdispf("Appending extension %s to %s would overflow the buffer, canceling", enable[i].extension, toggled);
                    atomic_write(cancel, 1);
                }
                if(!md5sum(toggled, enable[i].checksum))
                    update_success = false;
            }
            else if(!md5sum(enable[i].name, enable[i].checksum))
                    update_success = false;

            is_canceled = atomic_read(cancel);
            progress_increment(pc);
        }
        #pragma omp for schedule(dynamic)
        for(i = 0; i < disable_size; i++) {
            if(is_canceled || (!cfg->edain_available && strstr(disable[i].name, "edain")))
                continue;

            if(!file_exists(disable[i].name)) {
                config_enabled = true;
                strcpy(toggled, disable[i].name);
                if(set_extension(toggled, disable[i].extension, sizeof toggled) < 0) {
                    errdispf("Appending extension %s to %s would overflow the buffer, canceling", disable[i].extension, toggled);
                    atomic_write(cancel, 1);
                }
                if(!md5sum(toggled, disable[i].checksum))
                    update_success = false;
            }
            else if(!md5sum(disable[i].name, disable[i].checksum))
                    update_success = false;

            is_canceled = atomic_read(cancel);
            progress_increment(pc);
        }
        #pragma omp for schedule(dynamic)
        for(i = 0; i < swap_size; i++) {
            if(is_canceled)
                continue;

            if(swap[i].state == active) {
                if(config_enabled && !invert_dat_files) {
                    if(!md5sum(swap[i].name, swap[i].checksum))
                        update_success = false;
                }
                else {
                    if(!md5sum(swap[i].disabled, swap[i].checksum))
                        update_success = false;
                }
            }
            else {
                if(config_enabled && !invert_dat_files) {
                    if(!md5sum(swap[i].disabled, swap[i].checksum))
                        update_success = false;
                }
                else if(!md5sum(swap[i].name, swap[i].checksum))
                        update_success = false;
            }

            is_canceled = atomic_read(cancel);
            progress_increment(pc);
        }

    }

    if(atomic_read(cancel)) {
        free(enable);
        free(disable);
        free(swap);
        return true;
    }

    if(update_success) {
        write_success = write_game_config(filename, enable, enable_size,
                                                    disable, disable_size,
                                                    swap, swap_size);
    }
    else {
        errdispf("Errors were encountered during hashing of %s, config file will remain unchanged\n", filename);
        write_success = false;
    }

    free(enable);
    free(disable);
    free(swap);

    return update_success && write_success;
}

bool write_launcher_config(struct launcher_data const* cfg, char const* file) {
    char mount[sizeof cfg->mount_cmd];
    char umount[sizeof cfg->umount_cmd];

    if(toml_format(mount, cfg->mount_cmd, sizeof mount) < 0) {
        errdisp("Mount command overflowed the toml buffer");
        return false;
    }
    if(toml_format(umount, cfg->umount_cmd, sizeof umount) < 0) {
        errdisp("Umount command overflowed the toml buffer");
        return false;
    }

    FILE* fp = fopen(file, "w");

    if(!fp) {
        errdisp("Could not write config file\n");
        return false;
    }

    fprintf(fp, "[launcher]\n");
    fprintf(fp, "swap = \"%s\"\n", cfg->swap_dat_file ? "true" : "false");
    fprintf(fp, "kill_on_launch = \"%s\"\n", cfg->kill_on_launch ? "true" : "false");
    fprintf(fp, "show_console = \"%s\"\n", cfg->show_console ? "true" : "false");
    fprintf(fp, "default_state = \"%d\"\n", trailing_zerobits(cfg->default_state));
    fprintf(fp, "patch_version = \"%s\"\n", cfg->patch_version);
    fprintf(fp, "verify_active = \"%s\"\n\n", cfg->verify_active ? "true" : "false");
    fprintf(fp, "[game]\n");
    fprintf(fp, "path = \"%s\"\n\n", cfg->game_path);
    fprintf(fp, "[edain]\n");
    fprintf(fp, "available = \"%s\"\n\n", cfg->edain_available ? "true" : "false");
    fprintf(fp, "[botta]\n");
    fprintf(fp, "available = \"%s\"\n", cfg->botta_available ? "true" : "false");
    fprintf(fp, "path = \"%s\"\n\n", cfg->botta_path);
    fprintf(fp, "[mount]\n");
    fprintf(fp, "automatic = \"%s\"\n", cfg->automatic_mount ? "true" : "false");
    fprintf(fp, "mount_exe = \"%s\"\n", cfg->mount_exe);
    fprintf(fp, "disc_image = \"%s\"\n", cfg->disc_image);
    fprintf(fp, "mount_flags = \"%s\"\n", cfg->mount_flags);
    fprintf(fp, "umount_flags = \"%s\"\n", cfg->umount_flags);
    fprintf(fp, "mount_cmd = \"%s\"\n", mount);
    fprintf(fp, "umount_cmd = \"%s\"\n", umount);
    fprintf(fp, "umount_imspec = \"%s\"\n", cfg->umount_imspec ? "true" : "false");

    fclose(fp);

    return true;
}

bool read_launcher_config(struct launcher_data* cfg, char const* file) {
    launcher_data_init(cfg);

    FILE* fp = fopen(file, "r");
    if(!fp)
        return false;

    char line[PATH_SIZE];
    char header[HEADER_SIZE];
    char key[HEADER_SIZE];
    char value[PATH_SIZE];
    enum line_contents contents;
    unsigned line_number = 0;

    while(fgets(line, sizeof line, fp)) {
        ++line_number;
        contents = determine_line_contents(line);
        if(contents == content_invalid) {
            errdispf("Syntax error on line %u in %s: %s\n", line_number, file, line);
            fclose(fp);
            return false;
        }
        if(contents == content_blank)
            continue;

        if(contents == content_header) {
            header_name(line, header);
            continue;
        }
        if(contents == content_subheader) {
            subheader_name(line, header);
            continue;
        }
        get_table_key(line, key);
        get_table_value(line, value);

        if(strcmp(header, "launcher") == 0) {
            if(strcmp(key, "swap") == 0)
                cfg->swap_dat_file = strcmp(value, "true") == 0;
            else if(strcmp(key, "kill_on_launch") == 0)
                cfg->kill_on_launch = strcmp(value, "true") == 0;
            else if(strcmp(key, "show_console") == 0)
                cfg->show_console = strcmp(value, "true") == 0;
            else if(strcmp(key, "default_state") == 0)
                cfg->default_state = (0x1 << atoi(value));
            else if(strcmp(key, "verify_active") == 0)
                cfg->verify_active = strcmp(value, "true") == 0;
            else if(strcmp(key, "patch_version") == 0) {
                if(strscpy(cfg->patch_version, value, sizeof cfg->patch_version) < 0) {
                    errdisp("Buffer overflow detected in patch version");
                    fclose(fp);
                    return false;
                }
            }
            else
                errdispf("Unknown key %s\n", key);
        }
        else if(strcmp(header, "game") == 0) {
            if(strcmp(key, "path") == 0) {
                if(strscpy(cfg->game_path, value, sizeof cfg->game_path) < 0) {
                    errdisp("Buffer overflow detected in game path\n");
                    fclose(fp);
                    return false;
                }
            }
            else
                errdispf("Unknown key %s\n", key);
        }
        else if(strcmp(header, "edain") == 0) {
            if(strcmp(key, "available") == 0)
                cfg->edain_available = strcmp(value, "true") == 0;
            else
                errdispf("Unknown key %s\n", key);
        }
        else if(strcmp(header, "botta") == 0) {
            if(strcmp(key, "available") == 0)
                cfg->botta_available = strcmp(value, "true") == 0;
            else if(strcmp(key, "path") == 0) {
                if(strscpy(cfg->botta_path, value, sizeof cfg->botta_path) < 0) {
                    errdisp("Buffer overflow detected in botta path\n");
                    fclose(fp);
                    return false;
                }
            }
            else
                errdispf("Unknown key %s\n", key);
        }
        else if(strcmp(header, "mount") == 0) {
            if(strcmp(key, "automatic") == 0)
                cfg->automatic_mount = strcmp(value, "true") == 0;
            else if(strcmp(key, "mount_exe") == 0) {
                if(strscpy(cfg->mount_exe, value, sizeof cfg->mount_exe) < 0) {
                    errdisp("Buffer overflow detected in mount exe\n");
                    fclose(fp);
                    return false;
                }
            }
            else if(strcmp(key, "disc_image") == 0) {
                if(strscpy(cfg->disc_image, value, sizeof cfg->disc_image) < 0) {
                    errdisp("Buffer overflow detected in disc image\n");
                    fclose(fp);
                    return false;
                }
            }
            else if(strcmp(key, "mount_flags") == 0) {
                if(strscpy(cfg->mount_flags, value, sizeof cfg->mount_flags) < 0) {
                    errdisp("Buffer overflow detected in mount flags\n");
                    fclose(fp);
                    return false;
                }
            }
            else if(strcmp(key, "umount_flags") == 0) {
                if(strscpy(cfg->umount_flags, value, sizeof cfg->umount_flags) < 0) {
                    errdisp("Buffer overflow detected in umount flags\n");
                    fclose(fp);
                    return false;
                }
            }
            else if(strcmp(key, "mount_cmd") == 0) {
                if(sys_format(cfg->mount_cmd, value, sizeof cfg->mount_cmd)  < 0) {
                    errdisp("Buffer overflow detected in mount command\n");
                    fclose(fp);
                    return false;
                }
            }
            else if(strcmp(key, "umount_cmd") == 0) {
                if(sys_format(cfg->umount_cmd, value, sizeof cfg->umount_cmd)  < 0) {
                    errdisp("Buffer overflow detected in umount command\n");
                    fclose(fp);
                    return false;
                }
            }
            else if(strcmp(key, "umount_imspec") == 0)
                cfg->umount_imspec = strcmp(value, "true") == 0;
            else
                errdispf("Unknown key %s\n", key);
        }
        else
            errdispf("Unknown header %s\n", header);
    }

    fclose(fp);

    return true;
}

void construct_mount_command(char* dst, char const* exe, char const* flags, char const* img) {
    if(flags[0])
        sprintf(dst, "\'%s %s\' \'%s\'"SUPPRESS_OUTPUT, exe, flags, img);
    else
        sprintf(dst, "\'%s\' \'%s\'"SUPPRESS_OUTPUT, exe, img);
}

void construct_umount_command(char* dst, char const* exe, char const* flags, char const* img, bool spec_img) {
    if(flags[0] && spec_img)
        sprintf(dst, "\'%s\' \'%s\' \'%s\'"SUPPRESS_OUTPUT, exe, flags, img);
    else if(flags[0])
        sprintf(dst, "\'%s\' \'%s\'"SUPPRESS_OUTPUT, exe, flags);
    else if(spec_img)
        sprintf(dst, "\'%s\' \'%s\'"SUPPRESS_OUTPUT, exe, img);
    else
        sprintf(dst, "\'%s\'"SUPPRESS_OUTPUT, exe);
}

static void header_name(char* line, char* header) {
    char* str = trim_whitespace(line);
    size_t size = strlen(str);
    memcpy(header, str + 1, size - 2);
    header[size - 2] = '\0';
}

static void subheader_name(char* line, char* header) {
    char* str = trim_whitespace(line);
    size_t size = strlen(str);
    memcpy(header, str + 2, size - 4);
    header[size - 4] = '\0';
}

static void get_table_key(char* entry, char* key) {
    char* str = trim_whitespace(entry);
    size_t i;
    for(i = 0; i < strlen(str); i++) {
        if(isspace(str[i]))
            break;
    }
    memcpy(key, str, i);
    key[i] = '\0';
}

static void get_table_value(char const* entry, char* value) {
    char* start = strchr(entry, '"') + 1;
    char* end = strchr(start, '"');
    memcpy(value, start, end - start);
    value[end - start] = '\0';
}

static bool read_big_entry(char* line, struct big_file* entry) {
    if(determine_line_contents(line) != content_key_value_pair)
        return false;

    char key[HEADER_SIZE];
    char value[ENTRY_SIZE];

    get_table_key(line, key);
    get_table_value(line, value);

    if(strcmp(key, "name") == 0) {
        if(strscpy(entry->name, value, sizeof entry->name) < 0) {
            errdisp("Buffer overflow detected for big file name\n");
            return false;
        }
    }
    else if(strcmp(key, "checksum") == 0) {
        if(strscpy(entry->checksum, value, sizeof entry->checksum) < 0) {
            errdisp("Buffer overflow detected for big file checksum\n");
            return false;
        }
    }
    else if(strcmp(key, "extension") == 0) {
        if(strscpy(entry->extension, value, sizeof entry->extension) < 0) {
            errdisp("Buffer overflow detected for big file extension\n");
            return false;
        }
    }
    else
        return false;

    return true;
}

static bool read_dat_entry(char* line, struct dat_file* entry) {
    if(determine_line_contents(line) != content_key_value_pair)
        return false;

    char key[HEADER_SIZE];
    char value[ENTRY_SIZE];

    get_table_key(line, key);
    get_table_value(line, value);

    if(strcmp(key, "name") == 0) {
        if(strscpy(entry->name, value, sizeof entry->name) < 0) {
            errdisp("Buffer overflow detected in dat file name\n");
            return false;
        }
    }
    else if(strcmp(key, "checksum") == 0) {
        if(strscpy(entry->checksum, value, sizeof entry->checksum) < 0) {
            errdisp("Buffer overflow detected in dat file checksum\n");
            return false;
        }
    }
    else if(strcmp(key, "disabled") == 0) {
        if(strscpy(entry->disabled, value, sizeof entry->disabled) < 0) {
            errdisp("Buffer overflow detected in dat file disabled\n");
            return false;
        }
    }
    else if(strcmp(key, "introduced") == 0) {
        if(strscpy(entry->introduced, value, sizeof entry->introduced) < 0) {
            errdisp("Buffer overflow detected in dat file introduced\n");
            return false;
        }
    }
    else
        return false;

    return true;
}
