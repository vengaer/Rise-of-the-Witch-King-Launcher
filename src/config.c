#include "config.h"
#include "atomic.h"
#include "bitop.h"
#include "concurrency_utils.h"
#include "fsys.h"
#include "input.h"
#include <ctype.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BIG_TABLE_SIZE 3

static int progress = 0, total_work = -1;

void header_name(char* line, char* header);
void subheader_name(char* line, char* header);
void get_table_key(char* entry, char* key); 
void get_table_value(char const* entry, char* value);
bool read_big_entry(char* line, struct big_file* entry);
bool read_dat_entry(char* line, struct dat_file* entry);
char* trim_whitespace(char* str);

void prepare_progress(void) {
    atomic_write(&progress, 0);
    atomic_write(&total_work, 0);
}

void reset_progress(void) {
    atomic_write(&progress, 0);
    atomic_write(&total_work, -1);
}

double track_progress(void) {
    double const cp = (double) atomic_read(&progress);
    double const total = (double) atomic_read(&total_work);
    return cp / total;
}

void read_game_config(char const* filename,
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
        SAFE_FPRINTF(stderr, "%s could not be opened\n", filename)
        return;
    }
    else {
        fseek(fp, 0, SEEK_END);
        size_t file_size = ftell(fp);
        if(file_size == 0) {
            SAFE_FPRINTF(stderr, "%s is empty\n", filename)
            fclose(fp);
            return;
        }
    }

    rewind(fp);
    while(fgets(line, sizeof line, fp)) {
        ++line_number;
        contents = determine_line_contents(line);

        if(contents == content_invalid) {
            SAFE_FPRINTF(stderr, "Syntax error on line %u in %s: %s\n", line_number, filename, line);
            fclose(fp);
            return;
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
                SAFE_FPRINTF(stderr, "Syntax error on line %u in %s: %s\nUnknown subheader\n", line_number, filename, line);
                fclose(fp);
                return;
            }

            continue;
        }
        else {
            if(strcmp(subheader, "enable") == 0) {
                if(*enable_size >= *enable_capacity) {
                    *enable = realloc(*enable, 2 * (*enable_capacity) * sizeof(struct big_file));
                    *enable_capacity *= 2;
                }

                if(!read_big_entry(line, &(*enable)[(*enable_size) - 1])) {
                    SAFE_FPRINTF(stderr, "Syntax error on line %u in %s: %s\n", line_number, filename, line);
                    fclose(fp);
                    return;
                }
            }
            else if(strcmp(subheader, "disable") == 0) {
                if(*disable_size >= *disable_capacity) {
                    *disable = realloc(*disable, 2 * (*disable_capacity) * sizeof(struct big_file));
                    *disable_capacity *= 2;
                }

                if(!read_big_entry(line, &(*disable)[(*disable_size) - 1])) {
                    SAFE_FPRINTF(stderr, "Syntax error on line %u in %s: %s\n", line_number, filename, line);
                    fclose(fp);
                    return;
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
                }
                if(!read_dat_entry(line, &(*swap)[(*swap_size) - 1])) {
                    SAFE_FPRINTF(stderr, "Missing entry for %s in %s\n", header, filename)
                    fclose(fp);
                    return;
                }
            }
            else {
                SAFE_FPRINTF(stderr, "Unknown header %s\n", header)
                fclose(fp);
                return;
            }
        }
    }

    fclose(fp);
}

void write_game_config(char const* filename, 
                       struct big_file* enable, 
                       size_t enable_size, 
                       struct big_file* disable,
                       size_t disable_size,
                       struct dat_file* swap,
                       size_t swap_size) {
    FILE* fp = fopen(filename, "w");
    if(!fp) {
        SAFE_FPRINTF(stderr, "%s could not be opened\n", filename)
        return;
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
    }

    fclose(fp);
}

bool update_game_config(char const* filename, bool invert_dat_files, struct latch* latch, struct launcher_data const* cfg) {
    size_t enable_size, disable_size, swap_size;
    size_t enable_cap = 64, disable_cap = 64, swap_cap = 2;
    struct big_file* enable = malloc(enable_cap * sizeof(struct big_file));
    struct big_file* disable = malloc(disable_cap * sizeof(struct big_file));
    struct dat_file* swap = malloc(swap_cap * sizeof(struct dat_file));

    read_game_config(filename, &enable, &enable_cap, &enable_size,
                               &disable, &disable_cap, &disable_size,
                               &swap, &swap_cap, &swap_size);
    
    atomic_add(&total_work, enable_size + disable_size + swap_size);

    latch_count_down(latch);

    bool success = true;

    #pragma omp parallel reduction(&& : success)
    {
        size_t i;
        char toggled[FSTR_SIZE];
        bool config_enabled;

        #pragma omp for schedule(dynamic)
        for(i = 0; i < enable_size; i++) {
            if(!cfg->edain_available && strstr(enable[i].name, "edain"))
                continue;

            if(!file_exists(enable[i].name)) {
                config_enabled = false;
                strcpy(toggled, enable[i].name);
                set_extension(toggled, enable[i].extension);
                if(!md5sum(toggled, enable[i].checksum))
                    success = false;
            }
            else if(!md5sum(enable[i].name, enable[i].checksum))
                    success = false;

            atomic_inc(&progress);
        }
        #pragma omp for schedule(dynamic)
        for(i = 0; i < disable_size; i++) {
            if(!cfg->edain_available && strstr(disable[i].name, "edain"))
                continue;

            if(!file_exists(disable[i].name)) {
                config_enabled = true;
                strcpy(toggled, disable[i].name);
                set_extension(toggled, disable[i].extension);
                if(!md5sum(toggled, disable[i].checksum))
                    success = false;
            }
            else if(!md5sum(disable[i].name, disable[i].checksum))
                    success = false;

            atomic_inc(&progress);
        }
        #pragma omp for schedule(dynamic)
        for(i = 0; i < swap_size; i++) {
            strcpy(toggled, swap[i].name);
            set_extension(toggled, OTHER_EXT);

            if(swap[i].state == active) {
                if(config_enabled && !invert_dat_files) {
                    if(!md5sum(swap[i].name, swap[i].checksum))
                        success = false;
                }
                else {
                    if(!md5sum(toggled, swap[i].checksum))
                        success = false;
                }
            }
            else {
                if(config_enabled && !invert_dat_files) {
                    if(!md5sum(toggled, swap[i].checksum))
                        success = false;
                }
                else if(!md5sum(swap[i].name, swap[i].checksum))
                        success = false;
            }

            atomic_inc(&progress);
        }

    }

    if(success) {
        write_game_config(filename, enable, enable_size, 
                                    disable, disable_size, 
                                    swap, swap_size);
    }
    else 
        SAFE_FPRINTF(stderr, "Errors were encountered during hashing of %s, config file will remain unchanged\n", filename)

    free(enable);
    free(disable);
    free(swap);

    return success;
}

void write_launcher_config(struct launcher_data const* cfg, char const* file) {
    FILE* fp = fopen(file, "w");

    if(!fp) {
        fprintf(stderr, "Could not write config file\n");
        return;
    }

    fprintf(fp, "[launcher]\n");
    fprintf(fp, "swap = \"%s\"\n", cfg->swap_dat_file ? "true" : "false");
    fprintf(fp, "kill_on_launch = \"%s\"\n", cfg->kill_on_launch ? "true" : "false");
    fprintf(fp, "show_console = \"%s\"\n", cfg->show_console ? "true" : "false");
    fprintf(fp, "default_state = \"%d\"\n\n", trailing_zerobits(cfg->default_state));
    fprintf(fp, "verify_active = \"%s\"\n", cfg->verify_active ? "true" : "false");
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
    fprintf(fp, "mount_cmd = \"%s\"\n", cfg->mount_cmd);
    fprintf(fp, "umount_cmd = \"%s\"\n", cfg->umount_cmd);
    fprintf(fp, "umount_imspec = \"%s\"\n", cfg->umount_imspec ? "true" : "false");

    fclose(fp);
}

bool read_launcher_config(struct launcher_data* cfg, char const* file) {
    launcher_data_init(cfg);

    FILE* fp = fopen(file, "r");
    if(!fp) 
        return false;

    char line[PATH_SIZE];
    char header[HEADER_SIZE];
    char key[OPT_SIZE];
    char value[PATH_SIZE];
    enum line_contents contents;
    unsigned line_number = 0;

    while(fgets(line, sizeof line, fp)) {
        ++line_number;
        contents = determine_line_contents(line);
        if(contents == content_invalid) {
            SAFE_FPRINTF(stderr, "Syntax error on line %u in %s: %s\n", line_number, file, line);
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
            else
                fprintf(stderr, "Unknown key %s.\n", key);
        }
        else if(strcmp(header, "game") == 0) {
            if(strcmp(key, "path") == 0) 
                strcpy(cfg->game_path, value);
            else
                fprintf(stderr, "Unknown key %s.\n", key);
        }
        else if(strcmp(header, "edain") == 0) {
            if(strcmp(key, "available") == 0)
                cfg->edain_available = strcmp(value, "true") == 0;
            else
                fprintf(stderr, "Unknown key %s.\n", key);
        }
        else if(strcmp(header, "botta") == 0) {
            if(strcmp(key, "available") == 0)
                cfg->botta_available = strcmp(value, "true") == 0;
            else if(strcmp(key, "path") == 0)
                strcpy(cfg->botta_path, value);
            else
                fprintf(stderr, "Unknown key %s.\n", key);
        }
        else if(strcmp(header, "mount") == 0) {
            if(strcmp(key, "automatic") == 0)
                cfg->automatic_mount = strcmp(value, "true") == 0;
            else if(strcmp(key, "mount_exe") == 0)
                strcpy(cfg->mount_exe, value);
            else if(strcmp(key, "disc_image") == 0)
                strcpy(cfg->disc_image, value);
            else if(strcmp(key, "mount_flags") == 0)
                strcpy(cfg->mount_flags, value);
            else if(strcmp(key, "umount_flags") == 0)
                strcpy(cfg->umount_flags, value);
            else if(strcmp(key, "mount_cmd") == 0)
                sys_format(cfg->mount_cmd, value);
            else if(strcmp(key, "umount_cmd") == 0)
                sys_format(cfg->umount_cmd, value);
            else if(strcmp(key, "umount_imspec") == 0)
                cfg->umount_imspec = strcmp(value, "true") == 0;
            else
                fprintf(stderr, "Unknown key %s.\n", key);
        }
        else 
            fprintf(stderr, "Unknown header %s\n", header);
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

void header_name(char* line, char* header) {
    char* str = trim_whitespace(line);
    size_t size = strlen(str);
    memcpy(header, str + 1, size - 2);
    header[size - 2] = '\0';
}

void subheader_name(char* line, char* header) {
    char* str = trim_whitespace(line);
    size_t size = strlen(str);
    memcpy(header, str + 2, size - 4);
    header[size - 4] = '\0';
}

void get_table_key(char* entry, char* key) {
    char* str = trim_whitespace(entry);
    size_t i;
    for(i = 0; i < strlen(str); i++) {
        if(isspace(str[i]))
            break;
    }
    memcpy(key, str, i);
    key[i] = '\0';
}

void get_table_value(char const* entry, char* value) {
    char* start = strchr(entry, '"') + 1;
    char* end = strchr(start, '"');
    memcpy(value, start, end - start);
    value[end - start] = '\0';
}

bool read_big_entry(char* line, struct big_file* entry) {
    if(determine_line_contents(line) != content_key_value_pair)
        return false;

    char key[OPT_SIZE];
    char value[FSTR_SIZE];

    get_table_key(line, key);
    get_table_value(line, value);

    if(strcmp(key, "name") == 0)
        strcpy(entry->name, value);
    else if(strcmp(key, "checksum") == 0)
        strcpy(entry->checksum, value);
    else if(strcmp(key, "extension") == 0)
        strcpy(entry->extension, value);
    else 
        return false;

    return true;
}

bool read_dat_entry(char* line, struct dat_file* entry) {
    if(determine_line_contents(line) != content_key_value_pair)
        return false;

    char key[OPT_SIZE];
    char value[FSTR_SIZE];

    get_table_key(line, key);
    get_table_value(line, value);

    if(strcmp(key, "name") == 0)
        strcpy(entry->name, value);
    else if(strcmp(key, "checksum") == 0)
        strcpy(entry->checksum, value);
    else 
        return false;

    return true;
}

void replace_char(char* line, char orig, char repl) {
    size_t i;
    for(i = 0; i < strlen(line); i++) {
        if(line[i] == orig)
            line[i] = repl;
    }
}

char* trim_whitespace(char* str) {
    char* end;
    
    while(isspace((unsigned char)*str))
        ++str;
    
    if(*str == '\0')
        return str;

    end = str + strlen(str) - 1;

    while(end > str && isspace((unsigned char)*end))
        --end;
    end[1] = '\0';

    return str;
}

