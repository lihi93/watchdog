#include <stdlib.h> /* malloc */
#include <assert.h> /* assert */

#include "heap.h"
#include "d_vector.h"

#define INIT_CAPAC (20)
#define ROOT (0)
#define VECTOR (heap->vec)
#define LEFT_CHILD_INDEX(index) (2 * (index) + 1)
#define RIGHT_CHILD_INDEX(index) (2 * (index) + 2)
#define PARENT_INDEX(index) (((index) - 1) / 2)

/********** STRUCT ***********/

struct heap 
{
	vector_t *vec;
    int (*cmp_func)(const void *, const void *);
};

/**** FORWARD DECLARATIONS ****/

static void HeapifyDown(heap_t *heap, size_t index);
static void HeapifyUp(heap_t *heap, size_t index);
static void SwapElement(vector_t *vec, size_t index1, size_t index2);
static void *GetData(heap_t *heap, size_t index);

/****** USER FUNCTIONS *******/

heap_t *HeapCreate(int (*cmp_func)(const void *, const void *))
{
	heap_t *new_heap = malloc(sizeof(heap_t));
	
	if (NULL == new_heap)
	{
		return NULL;
	}
	
	new_heap->vec = VectorCreate(INIT_CAPAC, sizeof(void *));
	if (NULL == new_heap->vec)
	{
		free(new_heap);
		return NULL;
	}
	new_heap->cmp_func = cmp_func;
	
	return new_heap;
}


void HeapDestroy(heap_t *heap)
{
	assert(NULL != heap);
	
	VectorDestroy(heap->vec);
	free(heap);
}


void *HeapPeek(const heap_t *heap)
{
	assert(NULL != heap);
		
	return GetData((heap_t *)heap, ROOT);
}

int HeapIsEmpty(const heap_t *heap)
{
	assert(NULL != heap);
	
	return VectorIsEmpty(heap->vec);
}


size_t HeapSize(const heap_t *heap)
{
	assert(NULL != heap);
	
	return VectorSize(heap->vec);
}


void HeapPop(heap_t *heap)
{	
	assert(NULL != heap);
	SwapElement(VECTOR, ROOT, HeapSize(heap) - 1);
	VectorPopBack(heap->vec);
	HeapifyDown(heap, 0);
}
 

int HeapPush(heap_t *heap, void *data)
{
	int push_result = 0;
	size_t last_index = 0;
	
	assert(NULL != heap);
	
	push_result = VectorPushBack(VECTOR, (void *)data);
	last_index = HeapSize(heap) - 1;
	
	HeapifyUp(heap, last_index);
	
	return push_result;
}


void *HeapRemove(heap_t *heap, int (*is_match_func)(const void *data, const void *param), const void *param)
{
	size_t index = 0;
	size_t vec_end = HeapSize(heap) - 1;
	void *vec_data = GetData(heap, ROOT);
	void *to_rem = (void *)&param;
	
	while (index <= vec_end && 1 != is_match_func(vec_data, param))
	{
		++index;
		vec_data = GetData(heap, index);
	}
	
	if (index > vec_end)
	{
		return NULL;
	}

	SwapElement(VECTOR, index, vec_end);
	to_rem = vec_data;
	
	VectorPopBack(heap->vec);
	HeapifyDown(heap, index);
	
	return to_rem;
}

/****** INNER FUNCTIONS *******/

static void HeapifyDown(heap_t *heap, size_t index)
{
	 if ( (LEFT_CHILD_INDEX(index) < HeapSize(heap)) && (0 <  heap->cmp_func(GetData(heap, LEFT_CHILD_INDEX(index)),  GetData(heap, index))))
    {
        SwapElement(heap->vec, LEFT_CHILD_INDEX(index), index);
        HeapifyDown(heap, LEFT_CHILD_INDEX(index));
    }
    
    if ( (RIGHT_CHILD_INDEX(index) < HeapSize(heap)) && (0 <  heap->cmp_func(GetData(heap, RIGHT_CHILD_INDEX(index)), GetData(heap, index))))
    {
        SwapElement(heap->vec, RIGHT_CHILD_INDEX(index), index);
        HeapifyDown(heap, RIGHT_CHILD_INDEX(index));
    }
}


static void HeapifyUp(heap_t *heap, size_t index)
{
	size_t parent = PARENT_INDEX(index);

	if (index == 0)	
	{
		return;
	}
	
	if (heap->cmp_func(GetData(heap, parent), GetData(heap, index)) < 0)
	{
		SwapElement(VECTOR, parent, index);
	}
	
	HeapifyUp(heap, parent);
}


static void SwapElement(vector_t *vec, size_t index1, size_t index2)
{
    void **lhs = VectorGetAccessToElement(vec, index1);
    void **rhs = VectorGetAccessToElement(vec, index2);
    void  *temp = *lhs;
    *lhs = *rhs;
    *rhs = temp;
}


static void *GetData(heap_t *heap, size_t index)
{
	return *(void **)VectorGetAccessToElement(VECTOR, index);
}


