#include "atomic.h"
#include <omp.h>

#define SILENCE_LINT(x) (void)(x);

int atomic_inc(int* value) {
    int rv;
    #pragma omp flush
    
    #pragma omp atomic
    ++(*value);
    #pragma omp atomic read
    rv = *value;
    #pragma omp flush

    return rv;
}

int atomic_dec(int* value) {
    int rv;
    #pragma omp flush
    
    #pragma omp atomic
    --(*value);
    #pragma omp atomic read
    rv = *value;
    #pragma omp flush

    return rv;
}

int atomic_add(int* value, int inc) {
    int rv;
    #pragma omp flush
    
    #pragma omp atomic
    (*value) += inc;
    #pragma omp atomic read
    rv = *value;
    #pragma omp flush
    
    return rv;
}

int atomic_sub(int* value, int dec) {
    int rv;
    #pragma omp flush
    
    #pragma omp atomic
    (*value) -= dec;
    #pragma omp atomic read
    rv = *value;
    #pragma omp flush

    return rv;
}

int atomic_and(int* value, int rhs) {
    int rv;
    #pragma omp flush

    #pragma omp atomic
    (*value) &= rhs;
    #pragma omp atomic read
    rv = *value;

    #pragma omp flush

    return rv;
}

int atomic_or(int* value, int rhs) {
    int rv;
    #pragma omp flush
    
    #pragma omp atomic
    (*value) |= rhs;
    
    #pragma omp atomic read
    rv = *value;

    #pragma omp flush

    return rv;
}

int atomic_xor(int* value, int rhs) {
    int rv;
    #pragma omp flush
    
    #pragma omp atomic
    (*value) ^= rhs;
    #pragma omp atomic read
    rv = *value;

    #pragma omp flush

    return rv;
}

int atomic_fetch_add(int* value, int inc) {
    int rv;
    #pragma omp flush

    #pragma omp atomic read
    rv = *value;
    
    #pragma omp atomic
    (*value) += inc;

    #pragma omp flush
    
    return rv;
}

int atomic_fetch_sub(int* value, int dec) {
    int rv;
    #pragma omp flush
    
    #pragma omp atomic read
    rv = *value;

    #pragma omp atomic
    (*value) -= dec;

    #pragma omp flush

    return rv;
}

int atomic_fetch_and(int* value, int rhs) {
    int rv;
    #pragma omp flush

    #pragma omp atomic read
    rv = *value;

    #pragma omp atomic
    (*value) &= rhs;

    #pragma omp flush

    return rv;
}

int atomic_fetch_or(int* value, int rhs) {
    int rv;
    #pragma omp flush

    #pragma omp atomic read
    rv = *value;

    #pragma omp atomic
    (*value) |= rhs;

    #pragma omp flush

    return rv;
}

int atomic_fetch_xor(int* value, int rhs) {
    int rv;
    #pragma omp flush

    #pragma omp atomic read
    rv = *value;

    #pragma omp atomic
    (*value) ^= rhs;

    #pragma omp flush

    return rv;
}

int atomic_read(int* value) {
    int rv;
    #pragma omp flush
    #pragma omp atomic read
    rv = *value;

    return rv;
}

int atomic_write(int* value, int new_value) {
    SILENCE_LINT(new_value)
    int old;
    #pragma omp flush

    #pragma omp atomic read
    old = *value;
    #pragma omp atomic write
    *value = new_value;
    #pragma omp flush
    return old;
}
