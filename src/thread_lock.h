#ifndef THREAD_LOCK_H
#define THREAD_LOCK_H

#include <omp.h>

/* Spin lock */
#define TASKSYNC(x) \
_Pragma("omp atomic") \
--(*x); \
_Pragma("omp flush") \
while(*x) \
    sleep_for(100);

#define SAFE_FPRINTF(file, ...) \
_Pragma("omp critical(print_lock)") \
fprintf(file, __VA_ARGS__);

#endif