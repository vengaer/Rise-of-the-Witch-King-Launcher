#ifndef PROGRESS_CALLBACK_H
#define PROGRESS_CALLBACK_H

#include "atomic.h"

#ifdef __cplusplus
extern "C" {
#endif

struct progress_callback {
    int volatile total;
    int volatile current;
};

static inline void progress_init(struct progress_callback* pc) {
    atomic_write(&pc->total, 0);
    atomic_write(&pc->current, 0);
}

static inline void progress_increment(struct progress_callback* pc) {
    atomic_inc(&pc->current);
}

static inline void progress_set_total(struct progress_callback* pc, int new_total) {
    atomic_write(&pc->total, new_total);
}

static inline void progress_add_total(struct progress_callback* pc, int add) {
    atomic_add(&pc->total, add);
}

static inline void progress_set_current(struct progress_callback* pc, int new_current) {
    atomic_write(&pc->total, new_current);
}

static inline int progress_get_percentage(struct progress_callback const* pc) {
    double const current = atomic_read(&pc->current);
    int const total = atomic_read(&pc->total);
    double const den = total ? total : 1.0;

    return current / den * 100.0;
}
#ifdef __cplusplus
}
#endif

#endif
