#include <stdio.h> /* printf */
#include <stdlib.h> /* realloc */
#include <assert.h> /* assert */
#include <string.h> /* memcpy */
#include "d_vector.h"


#define GROWTH_FACTOR (2)	
#define REDUCE_RATIO (4)

struct dynamic_vector 
{
	void *data;
	size_t size;
	size_t capacity;
	size_t element_size;
};

vector_t *VectorCreate(size_t capacity, size_t element_size)
{
	vector_t *vector = (vector_t*)malloc(sizeof(vector_t));
	
	if(NULL == vector)
	{
		printf("vector malloc has failed\n");
		return NULL;
	}
	
	vector->capacity = capacity;
	vector->element_size = element_size;
	vector->size = 0; 
	
	vector->data = malloc(capacity * sizeof(void *));
	if(NULL == vector->data)
	{
		free(vector);
		printf("data malloc has failed\n");
		return NULL;
	}

	return vector;
}

void VectorDestroy(vector_t *vector)
{	
	assert(NULL != vector);
	
	/*memset(vector->data, 0, vector->capacity * vector->element_size);*/
	free(vector->data);

	/*memset((void *)vector, 0, sizeof(vector_t));*/
	free(vector);
}

status_t VectorPushBack(vector_t *vector, const void *element)
{
	int alloc_status = SUCCESS;
	void **place_to_push = NULL;
	
	assert(NULL != vector);
		
	if (vector->capacity - 1 == vector->size)
	{
		alloc_status = VectorReserve(vector, GROWTH_FACTOR * vector->capacity);
		vector->capacity = GROWTH_FACTOR * vector->capacity;
	}
	/*
	memcpy((char *)vector->data + (vector->element_size * vector->size), element, vector->element_size);
	*/
	
	place_to_push = (void **)((char *)(vector->data) + (vector->size * sizeof(void *)));
	*place_to_push = (void *)element;
	
	++vector->size;
	
	return alloc_status;
}

void VectorPopBack(vector_t *vector)
{
	void *temp_data = vector->data;
	
	assert(NULL != vector);
		
	--vector->size;
	
	if (vector->size <=  vector->capacity / REDUCE_RATIO)
	{
		temp_data = realloc(temp_data, (vector->capacity / GROWTH_FACTOR) * sizeof(void *));
		
		if (NULL != temp_data)
		{
			vector->data = temp_data;
			vector->capacity = (vector->capacity / GROWTH_FACTOR);
		}
	}
}

void *VectorGetAccessToElement(const vector_t *vector, size_t index)
{
	assert(NULL != vector);
	
	return ((char *)(vector->data) + (index * sizeof(void *)));
}

status_t VectorShrinkToSize(vector_t *vector)
{
	vector_t *temp_data = vector->data;
	status_t alloc_status = SUCCESS;
	
	assert(NULL != vector);
		
	temp_data = realloc(temp_data, vector->size + 1 * sizeof(void *));
		
	if (NULL == temp_data)
	{
		alloc_status = FAILURE;
	}
	else
	{
		vector->data = temp_data;
		vector->capacity = vector->size + 1;
	}
		
	return alloc_status;
}


status_t VectorReserve(vector_t *vector, size_t reserve_size)
{
	void *temp_data = vector->data; 
	
	assert(NULL != vector);
	
	if (reserve_size < vector->capacity)
	{
		return FAILURE;
	}
	
	temp_data = realloc(temp_data, reserve_size * sizeof(void *));
	
	if(NULL == temp_data)
	{
		printf("vector malloc has failed\n");
		return FAILURE;
	}
	
	vector->data = temp_data; 
	
	vector->capacity = reserve_size;
	
	return 0;
}

int VectorIsEmpty(const vector_t *vector)
{
	assert(NULL != vector);
	
	return 0 == vector->size;	
}

size_t VectorCapacity(const vector_t *vector)
{
	assert(NULL != vector);
		
	return vector->capacity;
}

size_t VectorSize(const vector_t *vector)
{
	assert(NULL != vector);
		
	return vector->size;
}


size_t GetElementSize(vector_t *vector)
{
	return vector->element_size;
}





