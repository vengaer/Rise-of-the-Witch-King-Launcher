#include "strutils.h"
#include <ctype.h>
#include <stddef.h>
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
