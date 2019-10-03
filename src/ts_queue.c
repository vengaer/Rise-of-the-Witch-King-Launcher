#include "ts_queue.h"
#include <stdlib.h>

static struct ts_queue_node* alloc_node(void* data) {
    struct ts_queue_node* n = malloc(sizeof(struct ts_queue_node));
    n->data = data;
    n->next = NULL;
    mutex_init(&n->mtx);
    return n;
}

static void free_node(struct ts_queue_node* n) {
    mutex_destroy(&n->mtx);
    free(n);
}

void queue_init(struct ts_queue* q) {
    q->head = q->tail = alloc_node(NULL);
}

void queue_destroy(struct ts_queue* q) {
    while(!queue_empty(q))
        queue_pop(q);
    free_node(q->head);
    q->head = q->tail = NULL;
}

bool queue_empty(struct ts_queue* q) {
    bool empty;
    mutex_lock(&q->head->mtx);
    empty = q->head->next == NULL;
    mutex_unlock(&q->head->mtx);
    return empty;
}

void queue_push(struct ts_queue* q, void* data) {
    struct ts_queue_node* prev;
    struct ts_queue_node* n = alloc_node(data);
    mutex_lock(&q->tail->mtx);
    q->tail->next = n;
    prev = q->tail;
    q->tail = q->tail->next;
    mutex_unlock(&prev->mtx);
}

void* queue_pop(struct ts_queue* q) {
    struct ts_queue_node* n;
    void* data;
    if(queue_empty(q))
        return NULL;

    mutex_lock(&q->head->mtx);
    mutex_lock(&q->head->next->mtx);
    n = q->head->next;
    q->head->next = n->next;
    mutex_unlock(&q->head->mtx);
    mutex_unlock(&n->mtx);
    data = n->data;
    free_node(n);
    return data;
}
