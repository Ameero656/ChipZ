#include <stdlib.h>
#include <stdio.h>

#include "vector.h"

vector_t* vector_init() {
	vector_t* vector;
	void** data = NULL;
	
	vector = (vector_t*) malloc(sizeof(vector_t));
	if (!vector) goto cleanup;

	vector->size = 0;
	vector->capacity = DEFAULT_CAPACITY;

	data = (void**) malloc(sizeof(void*) * vector->capacity);
	if (!data) goto cleanup;

	vector->data = data;

	return vector;

	cleanup:
		if (data) free(data); 
        if (vector) free(vector);
		return NULL;
}

void vector_free_shallow(vector_t* vector) {
	if (vector == NULL) return;
	
	free(vector->data);
	free(vector);
}

void vector_free(vector_t* vector) {
	if (vector == NULL) return;

	for (int i=0; i<vector->size; ++i) 
		free(vector->data[i]);
	
	free(vector->data);
	free(vector);
}



void vector_push_back(vector_t* vector, const void* value) {
	if (vector->size == vector->capacity) 
		vector_realloc(vector);

	vector->data[vector->size] = (void*) value;
	vector->size++;
}


void vector_set(vector_t* vector, const int i, const void* value) {
	if (!vector_is_valid_index(vector, i)) 
		return;

	vector->data[i] = (void*) value;
}

void vector_realloc(vector_t* vector) {
	void** data;

	int newCapacity = vector->capacity * 2;

	data = (void**) realloc(vector->data, sizeof(void*) * newCapacity);
	if (!data) 
		return;

	vector->capacity = newCapacity;
	vector->data = data;
	return;

	cleanup:
		free(data);

}

void* vector_get(const vector_t* vector, const int i) {
	if (vector_is_valid_index(vector, i)) 
		return vector->data[i];

	return NULL;
}