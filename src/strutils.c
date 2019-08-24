#include "strutils.h"
#include <ctype.h>
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

    dst[0] = '\0';

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
