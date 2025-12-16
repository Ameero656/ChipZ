#ifndef VECTOR_H
#define VECTOR_H

#include <stdbool.h>
#include <stdlib.h>

#define DEFAULT_CAPACITY 8

typedef struct vector {	
	int size;
	int capacity;
	void** data;
} vector_t;


vector_t* vector_init();

void vector_realloc(vector_t* vector);
void vector_free_shallow(vector_t* vector);
void vector_free(vector_t* vector);


void vector_push_back(vector_t* vector, const void* value);
void vector_set(vector_t* vector, const int i, const void* value);

inline static bool vector_is_valid_index(const vector_t* vector, const int i) {
	return (vector != NULL) && (i >= 0) && (i < vector->size);
}

void* vector_get(const vector_t* vector, const int i);

#endif