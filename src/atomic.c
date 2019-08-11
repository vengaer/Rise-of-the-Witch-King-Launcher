#include "atomic.h"
#include <omp.h>

int atomic_inc(int* value) {
    #pragma omp flush
    
    #pragma omp atomic
    ++(*value);
    #pragma omp flush
}

int atomic_dec(int* value) {
    #pragma omp flush
    
    #pragma omp atomic
    --(*value);
    #pragma omp flush
}

int atomic_inc_with(int* value, int inc) {
    #pragma omp flush
    
    #pragma omp atomic
    (*value) += inc;
    #pragma omp flush
}

int atomic_dec_with(int* value, int dec) {
    #pragma omp flush
    
    #pragma omp atomic
    (*value) -= dec;
    #pragma omp flush
}

int atomic_read(int* value) {
    #pragma omp flush
    #pragma omp atomic read
    int const rv = value;

    return rv;
}

int atomic_write(int* value, int new_value) {
    #pragma omp flush

    #pragma omp atomic read
    int const old = value;
    #pragma omp atomic write
    *value = new_value;
    #pragma omp flush
    return old;
}
