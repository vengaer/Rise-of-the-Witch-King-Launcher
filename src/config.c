#include "config.h"
#include "fsys.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool header_name(char const* line, char* header);
bool subheader_name(char const* line, char* header);
void get_table_key(char const* entry, char* key); void get_table_value(char const* entry, char* value);
bool read_big_entry(char* line, big_file* entry);
bool read_dat_entry(char* line, dat_file* entry);
bool read_big_table(FILE** fp, char* line, size_t line_size, big_file* entry);
bool read_dat_table(FILE** fp, char* line, size_t line_size, dat_file* entry);

void read_game_config(char const* filename,
                      big_file** enable,
                      size_t* enable_capacity,
                      size_t* enable_size,
                      big_file** disable,
                      size_t* disable_capacity,
                      size_t* disable_size,
                      dat_file** swap,
                      size_t* swap_capacity,
                      size_t* swap_size) {

    *enable_size = 0;
    *disable_size = 0;
    *swap_size = 0;

    FILE* fp = fopen(filename, "r");
    if(!fp) {
        fprintf(stderr, "%s could not be opened\n", filename);
        return;
    }

    char line[LINE_SIZE];
    char header[HEADER_SIZE];
    char tmp_header[HEADER_SIZE];
    
    do{ 
        if(fgets(line, sizeof line, fp) == NULL) {
            fprintf(stderr, "%s is empty\n", filename);
            return;
        }
    } while(line[0] == '\0');

    if(!header_name(line, tmp_header)) {
        fprintf(stderr, "%s does not start with a header", filename);
        return;
    }
    strcpy(header, tmp_header);
        

    while(fgets(line, sizeof line, fp)) {
        if(line[0] == '\n')
            continue;

        if(header_name(line, tmp_header)) {
            strcpy(header, tmp_header);
            continue;
        }

        if(strcmp(header, "enable") == 0) {
            if(*enable_size >= *enable_capacity) {
                *enable = realloc(*enable, 2 * (*enable_capacity) * sizeof(big_file));
                *enable_capacity *= 2;
            }
            
            if(!read_big_table(&fp, line, sizeof line, &(*enable)[(*enable_size)++])) {
                fprintf(stderr, "Missing entry for %s in %s\n", header, filename);
                fclose(fp);
                return;
            }
        }
        else if(strcmp(header, "disable") == 0) {
            if(*disable_size >= *disable_capacity) {
                *disable = realloc(*disable, 2 * (*disable_capacity) * sizeof(big_file));
                *disable_capacity *= 2;
            }
            
            if(!read_big_table(&fp, line, sizeof line, &(*disable)[(*disable_size)++])) {
                fprintf(stderr, "Missing entry for %s in %s\n", header, filename);
                fclose(fp);
                return;
            }

        }
        else if(strcmp(header, "swap") == 0) {
            if(*swap_size >= *swap_capacity) {
                *swap = realloc(*swap, 2 * (*swap_capacity) * sizeof(dat_file));
                *swap_capacity *= 2;
            }
            if(!read_dat_table(&fp, line, sizeof line, &(*swap)[(*swap_size)++])) {
                fprintf(stderr, "Missing entry for %s in %s\n", header, filename);
                fclose(fp);
                return;
            }
        }
        else {
            fprintf(stderr, "Unknown header %s\n", header);
            fclose(fp);
            return;
        }
    }
    
    fclose(fp);
}


void write_game_config(char const* filename, 
                       big_file* enable, 
                       size_t enable_size, 
                       big_file* disable,
                       size_t disable_size,
                       dat_file* swap,
                       size_t swap_size) {
    FILE* fp = fopen(filename, "w");
    if(!fp) {
        fprintf(stderr, "%s could not be opened\n", filename);
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


void write_launcher_config(launcher_data const* cfg, char const* file) {
    FILE* fp = fopen(file, "w");

    if(!fp) {
        fprintf(stderr, "Could not write config file\n");
        return;
    }

    fprintf(fp, "[launcher]\n");
    fprintf(fp, "swap = \"%s\"\n", cfg->swap_dat_file ? "true" : "false");
    fprintf(fp, "kill_on_launch = \"%s\"\n", cfg->kill_on_launch ? "true" : "false");
    fprintf(fp, "show_console = \"%s\"\n", cfg->show_console ? "true" : "false");
    fprintf(fp, "default_state = \"%d\"\n\n", cfg->default_state);
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

bool read_launcher_config(launcher_data* cfg, char const* file) {
    launcher_data_init(cfg);

    FILE* fp = fopen(file, "r");
    if(!fp) 
        return false;

    char line[PATH_SIZE];
    char header[HEADER_SIZE];
    char tmp_header[HEADER_SIZE];
    char key[OPT_SIZE];
    char value[PATH_SIZE];

    while(fgets(line, sizeof line, fp)) {
        if(line[0] == '\n')
            continue;

        if(subheader_name(line, tmp_header)) {
            strcpy(header, tmp_header);
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
                cfg->default_state = atoi(value);
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
    dst[0] = '\'';
    strcpy(dst + 1, exe);
    if(flags[0] != '\0') {
        strcat(dst, " ");
        strcat(dst, flags);
    }
    strcat(dst, "' '");
    strcat(dst, img);
    strcat(dst, "'");
    strcat(dst, SUPPRESS_OUTPUT);
}

void construct_umount_command(char* dst, char const* exe, char const* flags, char const* img, bool spec_img) {
    dst[0] = '\'';
    strcpy(dst + 1, exe);
    if(flags[0] != '\0') {
        strcat(dst, "' '");
        strcat(dst, flags);
    }

    if(spec_img) {
        strcat(dst, "' '");
        strcat(dst, img);
    }
    strcat(dst, "'");
    strcat(dst, SUPPRESS_OUTPUT);
}

bool header_name(char const* line, char* header) {
    size_t size = strlen(line);
    if(line[0] == '[' && line[size - 2] == ']') {
        memcpy(header, line + 2, size - 5);
        header[size - 5] = '\0';
        return true;
    }

    return false;
}

bool subheader_name(char const* line, char* header) {
    size_t size = strlen(line);
    while(line[0] == ' ') {
        ++line;
        --size;
    }
    if(line[0] == '[' && line[1] != '[' && line[size - 3] != ']' && line[size - 2] == ']') {
        memcpy(header, line + 1, size - 3);
        header[size - 3] = '\0';
        return true;
    }

    return false;
}

void get_table_key(char const* entry, char* key) {
    size_t i;
    size_t len = strlen(entry);
    for(i = 0; i < len; i++) {
        if(entry[i] != ' ' && entry[i] != '\t')
            break;
    }
    size_t start = i;
    for(; i < len; i++)
        if(entry[i] == ' ')
            break;

    memcpy(key, entry + start, i - start);
    key[i - start] = '\0';
}

void get_table_value(char const* entry, char* value) {
    char* start = strchr(entry, '"');
    ++start;
    char* end = strchr(start, '"');
    memcpy(value, start, end - start);
    value[end - start] = '\0';
}

bool read_big_entry(char* line, big_file* entry) {
    char key[OPT_SIZE];
    get_table_key(line, key);

    char value[FSTR_SIZE];
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

bool read_dat_entry(char* line, dat_file* entry) {
    char key[OPT_SIZE];
    get_table_key(line, key);

    char value[FSTR_SIZE];
    get_table_value(line, value);

    if(strcmp(key, "name") == 0)
        strcpy(entry->name, value);
    else if(strcmp(key, "checksum") == 0)
        strcpy(entry->checksum, value);
    else 
        return false;
    
    return true;
}

bool read_big_table(FILE** fp, char* line, size_t line_size, big_file* entry) {
    int i;
    if(!read_big_entry(line, entry))
        return false;
    
    for(i = 0; i < 2; i++) {
        do{
            if(fgets(line, line_size, *fp) == NULL)
                return false;
        } while(line[0] == '\n');


        if(!read_big_entry(line, entry))
            return false;
    }

    return true;
}

bool read_dat_table(FILE** fp, char* line, size_t line_size, dat_file* entry) {
    int i;
    char subheader[HEADER_SIZE];
    char tmp_header[HEADER_SIZE];
    subheader_name(line, tmp_header);
    char* start = strchr(tmp_header, '.');
    ++start;
    strcpy(subheader, start);

    for(i = 0; i < 2; i++) {
        do{
            if(fgets(line, line_size, *fp) == NULL)
                return false;
        } while(line[0] == '\n');

        if(!read_dat_entry(line, entry)) {
            return false;
        }
    }

    if(strcmp(subheader, "activate") == 0)
        entry->state = active;
    else  
        entry->state = inactive;
    
    return true;
}

void remove_newline(char* line) {
    int len = strlen(line);
    if(line[len - 1] == '\n')
        line[len - 1] = '\0';
}

void replace_char(char* line, char orig, char repl) {
    int len = strlen(line);
    int i;
    for(i = 0; i < len; i++)
        if(line[i] == orig)
            line[i] = repl;
}

