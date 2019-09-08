#include "strutils.h"
#include "game_data.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

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

int strscpy(char* restrict dst, char const* restrict src, size_t count) {
    size_t const src_len = strlen(src);

    strncpy(dst, src, count);

    if(src_len >= count) {
        dst[count - 1] = '\0';
        return -E2BIG;
    }

    return src_len;
}

int strscat(char* restrict dst, char const* restrict src, size_t count) {
    size_t const src_len = strlen(src);
    size_t const dst_len = strlen(dst);
    size_t const remaining = count - dst_len;

    strncat(dst, src, remaining);


    if(src_len >= remaining) {
        dst[count - 1] = '\0';
        return -E2BIG;
    }

    return src_len;
}

int strscatf(char* restrict dst, size_t count, char const* restrict fmt, ...) {
    size_t const dst_len = strlen(dst);

    va_list args;
    va_start(args, fmt);
    vsnprintf(dst + dst_len, count - dst_len, fmt, args);
    va_end(args);

    /* Not null-terminated, formatted string was too long */
    if(dst[count - 1]) {
        dst[count - 1] = '\0';
        return -E2BIG;
    }

    return strlen(dst) - dst_len;
}

int sys_format(char* restrict dst, char const* restrict command, size_t dst_size) {
    #if defined __CYGWIN__ || defined _WIN32
        size_t const cmd_len = strlen(command);
        sprintf(dst, "\"%s\"", command);
        replace_char(dst, '\'', '\"');

        if(cmd_len >= dst_size) {
            dst[dst_size - 1] = '\0';
            return -E2BIG;
        }

        return cmd_len;
    #else

        size_t i, j;
        size_t const cmd_len = strlen(command);

        size_t upper_bound = cmd_len >= dst_size ? dst_size - 1 : cmd_len;

        /* Copy all chars but \' */
        for(i = 0, j = 0; i < upper_bound; i++) {
            if(command[i] != '\'')
                dst[j++] = command[i];
        }
        dst[j] = '\0';

        if(cmd_len >= dst_size)
            return -E2BIG;

        return (int) j - 1;
    #endif
}

int toml_format(char* restrict dst, char const* restrict command, size_t dst_size) {
    #if defined __CYGWIN__ || defined _WIN32
        size_t const actual_cmd_len = strlen(command) - 2;
        size_t  const size = actual_cmd_len >= dst_size ? dst_size - 1 : actual_cmd_len;
        memcpy(dst, command + 1, size);
        dst[size] = '\0';
        replace_char(dst, '\"', '\'');

        if(actual_cmd_len >= dst_size)
            return -E2BIG;

        return actual_cmd_len;
    #else
        bool is_ws = false;

        size_t i, j;
        dst[0] = '\'';
        for(i = 0, j = 1; i < strlen(command) && j < dst_size; i++) {
            if(command[i] == ' ' && !is_ws) {
                is_ws = true;
                dst[j++] = '\'';
            }
            else if(command[i] != ' ' && is_ws) {
                is_ws = false;
                dst[j++] = '\'';
            }
            dst[j++] = command[i];
        }
        /* Overflow */
        if(j + 1 >= dst_size) {
            dst[dst_size - 1] = '\0';
            return -E2BIG;
        }
        dst[j++] = '\'';
        dst[j] = '\0';
        return (int) j - 1;
    #endif
}


void errorfmt(char const* fmt, ...) {
    extern void(*display_error)(char const*);

    char buf[LINE_SIZE];

    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    if(buf[sizeof buf - 1])
        sprintf(buf, "Error message overflowed the buffer");

    display_error(buf);
}
