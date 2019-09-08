#ifndef STRUTILS_H
#define STRUTILS_H

#include <stddef.h>

#ifndef __cplusplus
#define RESTRICT restrict
#else
#define RESTRICT
#endif

#define E2BIG 7

#ifdef __cplusplus
extern "C" {
#endif

void replace_char(char* line, char orig, char repl);
char* trim_whitespace(char* str);
int strscpy(char* RESTRICT dst, char const* RESTRICT src, size_t count);
int strscat(char* RESTRICT dst, char const* RESTRICT src, size_t count);
int strscatf(char* RESTRICT dst, size_t count, char const* RESTRICT fmt, ...);

void errorfmt(char const* fmt, ...);

#ifdef __cplusplus
}
#endif

#endif
