#ifndef TS_QUEUE_H
#define TS_QUEUE_H

#include "mutex.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

struct ts_queue_node {
    void* data;
    struct ts_queue_node* next;
    mutex mtx;
};

struct ts_queue {
    struct ts_queue_node* volatile head;
    struct ts_queue_node* volatile tail;
};

/* init and destory must be called in sequential sections */
void queue_init(struct ts_queue* q);
void queue_destroy(struct ts_queue* q);

bool queue_empty(struct ts_queue* q);

void queue_push(struct ts_queue* q, void* data);
void* queue_pop(struct ts_queue* q);


#ifdef __cplusplus
}
#endif

#endif
