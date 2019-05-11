#include "config.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool header_name(char const* line, char* header);
bool subheader_name(char const* line, char* header);
void get_table_key(char const* entry, char* key);
void get_table_value(char const* entry, char* value);
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
        fprintf(stderr, "%s could not be opened", filename);
        return;
    }

    char line[128];
    char header[32];
    char tmp_header[32];
    
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
                *enable = realloc(*enable, 2 * (*enable_capacity));
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
                *disable = realloc(*disable, 2 * (*disable_capacity));
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
                *swap = realloc(*swap, 2 * (*swap_capacity));
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
        fprintf(stderr, "%s could not be opened", filename);
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
    for(i = 0; i < strlen(entry); i++) {
        if(isalpha(entry[i]))
            break;
    }
    size_t start = i;
    for(; i < strlen(entry); i++)
        if(!isalpha(entry[i]))
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
    char key[32];
    get_table_key(line, key);

    char value[64];
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
    char key[32];
    get_table_key(line, key);

    char value[64];
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
    char subheader[32];
    char tmp_header[32];
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
