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
    dst[count - 1] = '\0';

    if(src_len >= count)
        return -E2BIG;

    return src_len;
}

int strscat(char* restrict dst, char const* restrict src, size_t count) {
    size_t const src_len = strlen(src);
    size_t const dst_len = strlen(dst);
    size_t const remaining = count - dst_len;

    strncat(dst, src, remaining);

    dst[count - 1] = '\0';

    if(src_len >= remaining)
        return -E2BIG;

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

void errorfmt(char const* fmt, ...) {
    extern void(*display_error)(char const*);

    char buf[LINE_SIZE];

    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    display_error(buf);
}
