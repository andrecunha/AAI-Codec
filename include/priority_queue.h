#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include "pq_definitions.h"

struct priority_queue {
	unsigned int size;
	NODE_TYPE **nodes;
};

typedef struct priority_queue priority_queue;

void pq_init (priority_queue *pq, size_t max_size);

void pq_insert (priority_queue *pq, NODE_TYPE *node);

NODE_TYPE* pq_remove (priority_queue *pq);

void pq_destroy (priority_queue *pq);

int pq_is_empty (priority_queue *pq);

#endif /* PRIORITY_QUEUE_H */
