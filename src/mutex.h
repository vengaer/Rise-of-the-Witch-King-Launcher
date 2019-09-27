#ifndef MUTEX_H
#define MUTEX_H

#ifdef _OPENMP
#include <omp.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _OPENMP

typedef struct {
    omp_lock_t lock;
} mutex;

static inline void mutex_init(mutex* m) {
    omp_init_lock(&m->lock);
}

static inline void mutex_destroy(mutex* m) {
    omp_destroy_lock(&m->lock);
}

static inline void mutex_lock(mutex* m) {
    omp_set_lock(&m->lock);
}

static inline void mutex_unlock(mutex* m) {
    omp_unset_lock(&m->lock);
}

#else

typedef struct { } mutex;
static inline void mutex_init(mutex* m) { /* no-op */ }
static inline void mutex_destroy(mutex* m) { /* no-op */ }
static inline void mutex_lock(mutex* m) { /* no-op */ }
static inline void mutex_unlock(mutex* m) { /* no-op */ }

#endif

#ifdef __cplusplus
}
#endif

#endif
