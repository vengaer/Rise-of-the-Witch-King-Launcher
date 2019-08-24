#ifndef STRUTILS_H
#define STRUTILS_H

#ifdef __cplusplus
extern "C" {
#endif

void replace_char(char* line, char orig, char repl);
char* trim_whitespace(char* str);

#ifdef __cplusplus
}
#endif

#endif
