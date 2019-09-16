#include "atomic.h"

#define UNUSED(x) (void)(x);

int atomic_inc(int volatile* value) {
    int rv;
    #pragma omp flush
    
    #pragma omp atomic capture
    {
        ++(*value);
        rv = *value;
    }
    #pragma omp flush

    return rv;
}

int atomic_dec(int volatile* value) {
    int rv;
    #pragma omp flush
    
    #pragma omp atomic capture
    {
        --(*value);
        rv = *value;
    }
    #pragma omp flush

    return rv;
}

int atomic_add(int volatile* value, int inc) {
    int rv;
    #pragma omp flush
    
    #pragma omp atomic capture
    {
        (*value) += inc;
        rv = *value;
    }
    #pragma omp flush
    
    return rv;
}

int atomic_sub(int volatile* value, int dec) {
    int rv;
    #pragma omp flush
    
    #pragma omp atomic capture
    {
        (*value) -= dec;
        rv = *value;
    }
    #pragma omp flush

    return rv;
}

int atomic_and(int volatile* value, int rhs) {
    int rv;
    #pragma omp flush

    #pragma omp atomic capture
    {
        (*value) &= rhs;
        rv = *value;
    }

    #pragma omp flush

    return rv;
}

int atomic_or(int volatile* value, int rhs) {
    int rv;
    #pragma omp flush
    
    #pragma omp atomic capture
    {
        (*value) |= rhs;
        rv = *value;
    }

    #pragma omp flush

    return rv;
}

int atomic_xor(int volatile* value, int rhs) {
    int rv;
    #pragma omp flush
    
    #pragma omp atomic capture
    {
        (*value) ^= rhs;
        rv = *value;
    }

    #pragma omp flush

    return rv;
}

int atomic_fetch_add(int volatile* value, int inc) {
    int rv;
    #pragma omp flush

    #pragma omp atomic capture
    {
        rv = *value;
        (*value) += inc;
    }

    #pragma omp flush
    
    return rv;
}

int atomic_fetch_sub(int volatile* value, int dec) {
    int rv;
    #pragma omp flush
    
    #pragma omp atomic capture
    {
        rv = *value;
        (*value) -= dec;
    }

    #pragma omp flush

    return rv;
}

int atomic_fetch_and(int volatile* value, int rhs) {
    int rv;
    #pragma omp flush

    #pragma omp atomic capture
    {
        rv = *value;
        (*value) &= rhs;
    }

    #pragma omp flush

    return rv;
}

int atomic_fetch_or(int volatile* value, int rhs) {
    int rv;
    #pragma omp flush

    #pragma omp atomic capture
    {
        rv = *value;
        (*value) |= rhs;
    }

    #pragma omp flush

    return rv;
}

int atomic_fetch_xor(int volatile* value, int rhs) {
    int rv;
    #pragma omp flush

    #pragma omp atomic capture
    {
        rv = *value;
        (*value) ^= rhs;
    }

    #pragma omp flush

    return rv;
}

int atomic_read(int const volatile* value) {
    int rv;
    #pragma omp flush
    #pragma omp atomic read
    rv = *value;

    return rv;
}

int atomic_write(int volatile* value, int new_value) {
    UNUSED(new_value)
    int old;
    #pragma omp flush

    #pragma omp atomic read
    old = *value;
    #pragma omp atomic write
    *value = new_value;
    #pragma omp flush
    return old;
}
