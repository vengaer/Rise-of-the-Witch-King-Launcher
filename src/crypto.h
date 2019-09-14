#ifndef CRYPTO_H
#define CRYPTO_H
#include <stdbool.h>
#include "strutils.h" /* RESTRICT */

#ifdef __cplusplus
extern "C" {
#endif

bool md5sum(char const* RESTRICT filename, char* RESTRICT csum);

#ifdef __cplusplus
}
#endif

#endif
