#pragma once

struct dynamic_array {
	int size;
	int capacity;
	unsigned char* array;
	int element_size;
};

struct dynamic_array da_create(int element_size);
void da_insert(struct dynamic_array* da, const void* element, int element_size);
void da_clear(struct dynamic_array* da);
void* da_get(struct dynamic_array* da, int index);
void* da_set(struct dynamic_array* da, int index, void* element, int element_size);
void da_delete_first_n(struct dynamic_array* da, int n);