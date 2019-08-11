#ifndef LATCH_H
#define LATCH_H

#ifdef __cplusplus
extern "C" {
#endif

struct latch {
    unsigned volatile count;
};

void latch_init(struct latch* latch, unsigned count);

void latch_count_down(struct latch* latch);


#ifdef __cplusplus
}
#endif

#endif
