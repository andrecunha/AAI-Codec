#include <stdio.h>
#include <stdlib.h>
#include <priority_queue.h>

int pq_get_priority(NODE_TYPE *node)
{
	return *node;
}

int main (int argc, char* argv[])
{
	priority_queue *myPQ = malloc(sizeof(priority_queue));
	NODE_TYPE a=3, b=2, c=1, d=0, e=-1;

	pq_init(myPQ);

	pq_insert(myPQ, &a);
	pq_insert(myPQ, &b);
	pq_insert(myPQ, &c);
	pq_insert(myPQ, &d);
	pq_insert(myPQ, &e);

	while (!pq_is_empty(myPQ))
		printf("%d\n", *(pq_remove(myPQ)));

	free(myPQ);
	return 0;
}
