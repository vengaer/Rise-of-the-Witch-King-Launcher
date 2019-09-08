#include "progress_callback.h"
#include "atomic.h"

void progress_init(struct progress_callback* pc) {
    atomic_write(&pc->total, 0);
    atomic_write(&pc->current, 0);
}

void progress_increment(struct progress_callback* pc) {
    atomic_inc(&pc->current);
}

void progress_set_total(struct progress_callback* pc, int new_total) {
    atomic_write(&pc->total, new_total);
}

void progress_set_current(struct progress_callback* pc, int new_current) {
    atomic_write(&pc->total, new_current);
}

int progress_get_percentage(struct progress_callback const* pc) {
    double const current = atomic_read(&pc->current);
    double const total = atomic_read(&pc->total);

    return current / total * 100.0;
}
