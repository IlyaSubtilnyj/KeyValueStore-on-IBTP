#include "../pch.h"
#include "dynamic_array.h"

#include <assert.h>
#include <memory.h>
#include <malloc.h>
#include <errno.h>

struct dynamic_array da_create(int element_size) {
	return (struct dynamic_array) { .size = 0, .capacity = 0, .array = (void*)0, .element_size = element_size };
}

void da_insert(struct dynamic_array* da, const void* element, int element_size) {
	assert((element_size == da->element_size) && ("Bad element. Size is not the same as was declared"));
	const int start_capacity = 10;
	const float expension_factor = 1.5f;
	if (da->capacity == 0) {
		da->array = (unsigned char*)malloc(start_capacity * da->element_size);
		if (da->array == (void*)0) return;
		da->capacity = start_capacity;
	}
	else if (da->size == da->capacity) {
		int updated_capacity = da->capacity * expension_factor;
		unsigned char* updted_array = (unsigned char*)realloc(da->array, updated_capacity * da->element_size);
		if (da->array == (void*)0) return;
		da->array = updted_array;
		da->capacity = updated_capacity;
	}
	errno_t err = memcpy_s(da->array + da->size * da->element_size, da->element_size, element, da->element_size);
	if (err) return;
	da->size++;
}

void* da_get(struct dynamic_array* da, int index) {
	assert((da->size > index) && ("index >= size of dynamic array"));
	return da->array + index * da->element_size;
}

void* da_set(struct dynamic_array* da, int index, void* element, int element_size) {
	assert(da->element_size == element_size);
	assert((da->size > index) && ("index >= size of dynamic array"));
	errno_t err = memcpy_s(da->array + index * da->element_size, da->element_size, element, da->element_size);
	if (err) return;
}

void da_delete_first_n(struct dynamic_array* da, int n) {
	assert(da->size >= n);
	int new_size = (da->size - n);
	if (new_size == 0) 
	{
		free(da->array);
		da->array = (void*)0;
		da->capacity = 0;
		da->size = 0;
		da->element_size = 0;
	}
	else
	{
		unsigned char* new_array = (unsigned char*)malloc((new_size - n) * da->element_size);
		if (new_array == (void*)0) return;
		errno_t err = memcpy_s(new_array, new_size * da->element_size, da->array + n * da->element_size, new_size * da->element_size);
		if (err) return;
		free(da->array);
		da->array = new_array;
		da->capacity = new_size;
		da->size = new_size;
	}
}

void da_clear(struct dynamic_array* da) {
	if (da->size != 0) {
		free(da->array);
		da->size = 0;
		da->capacity = 0;
		da->element_size = 0;
	}
}