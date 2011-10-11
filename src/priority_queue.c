#include <stdlib.h>
#include <priority_queue.h>

#define parent(i) ((i-1)/2)
#define left_child(i) (2*i+1)
#define right_child(i) (2*i+2)

int min (priority_queue *pq, int a, int b, int c)
{
	int small;
	if (b < pq->size && pq_get_priority(pq->nodes[b]) < pq_get_priority(pq->nodes[a]))
		small = b;
	else
		small = a;

	if (c < pq->size && pq_get_priority(pq->nodes[small]) > pq_get_priority(pq->nodes[c]))
		small = c;

	return small;
}

void swap (priority_queue *pq, int a, int b)
{
	NODE_TYPE *aux = pq->nodes[a];
	pq->nodes[a] = pq->nodes[b];
	pq->nodes[b] = aux;
}

void heapify (priority_queue *pq, int n)
{
	int left, right, smallest;

	left = left_child(n);
	right = right_child(n);

	smallest = min(pq, n, left, right);

	if (smallest != n) {
		swap(pq, n, smallest);
		heapify(pq, smallest);
	}
}

void pq_init (priority_queue *pq, size_t max_size)
{
	pq->size = 0;
	pq->nodes = malloc(max_size*sizeof(NODE_TYPE*));
}

void pq_insert (priority_queue *pq, NODE_TYPE *node)
{
	pq->size++;
	int i = pq->size-1;

	while ((i>0) && (pq_get_priority(pq->nodes[parent(i)]) > pq_get_priority(node))) {
		pq->nodes[i] = pq->nodes[parent(i)];
		i = parent(i);
	}

	pq->nodes[i] = node;
}

NODE_TYPE* pq_remove (priority_queue *pq)
{
	NODE_TYPE *r;

	if(pq->size <= 0)
		return NULL;

	r = pq->nodes[0];

	pq->size--;

	pq->nodes[0] = pq->nodes[pq->size];
	
	heapify(pq, 0);

	return r;
}

void pq_destroy (priority_queue *pq)
{
	pq->size = 0;
	free(pq->nodes);
}

int pq_is_empty (priority_queue *pq)
{
	return !(pq->size);
}

