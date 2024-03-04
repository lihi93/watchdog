#include <stdlib.h> /* malloc */
#include <assert.h> /* assert */
#include "pq.h"
#include "heap.h"
#include "d_vector.h"

struct pq
{
	heap_t *heap;
};

pq_t *PQCreate(int (*priority_func)(const void *lhs, const void *rhs))
{
	pq_t *new_pq = NULL;
	
	assert(NULL != priority_func);
	
	new_pq = malloc(sizeof(pq_t));
	if (NULL == new_pq)
	{
		return NULL;
	}
	
	new_pq->heap = HeapCreate(priority_func);
	if (NULL == new_pq->heap)
	{
		free(new_pq);
		return NULL;
	}
	return new_pq;
}


void PQDestroy(pq_t *pq)
{
	assert(NULL != pq);
	HeapDestroy(pq->heap);
	free(pq);
}


int PQEnqueue(pq_t *pq, void *data)
{
	assert(NULL != pq);
	
	return HeapPush(pq->heap, data);
}


void *PQPeek(const pq_t *pq)
{
	assert(NULL != pq);
	
	return HeapPeek(pq->heap);
}


int PQIsEmpty(const pq_t *pq)
{
	assert(NULL != pq);
	
	return HeapIsEmpty(pq->heap);
}


size_t PQSize(const pq_t *pq)
{
	assert(NULL != pq);
	
	return HeapSize(pq->heap);
}


void *PQErase(pq_t *pq, int (*is_match)(const void *data, const void *param), void *param)
{
	assert(NULL != pq);
	assert(NULL != is_match);
	
	return HeapRemove(pq->heap, is_match, param);
}


void PQClear(pq_t *pq)
{
	assert(NULL != pq);
	
	while (!PQIsEmpty(pq))
	{
		HeapPop(pq->heap);
	}
}


void PQDequeue(pq_t *pq)
{
	HeapPop(pq->heap);
}




