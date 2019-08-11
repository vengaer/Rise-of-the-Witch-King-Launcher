#ifndef THREAD_LOCK_H
#define THREAD_LOCK_H

#include <omp.h>

#define SAFE_FPRINTF(file, ...) \
_Pragma("omp critical(SP_print_lock)") \
fprintf(file, __VA_ARGS__);

#endif
