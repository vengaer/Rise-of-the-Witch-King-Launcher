#include "latch.h"
#include "command.h"
#include <omp.h>

#define SILENCE_LINT(x) (void)(x);

void latch_init(struct latch* latch, unsigned count) {
    SILENCE_LINT(count)
    #pragma omp atomic write
    latch->count = count;

    #pragma omp flush
}

void latch_count_down(struct latch* latch) {
    unsigned tmp;
    #pragma omp flush

    #pragma omp atomic
    --latch->count;

    #pragma omp flush

    #pragma omp atomic read
    tmp = latch->count;

    while(tmp) {
        sleep_for(100);
        #pragma omp flush
        #pragma omp atomic read
        tmp = latch->count;
    }
}
