#include "map.h"

void clear(struct map* map) {
	map->size = 0;
}

void insert(struct map* map, struct mem_dump* key, struct mem_dump val) {
	int i;
	for (i = 0; i < map->size; i++) {
		if (!dump_cmp(&map->keys[i], key)) {
			// Key already exists, update the value
			map->keys[i] = *key;
			map->values[i] = val;
			return;
		}
	}
	// Key does not exist, add it to the map
	assert(map->size < MAX_MAP_SIZE);
	map->keys[i] = *key;
	map->values[i] = val;
	map->size++;
}

struct mem_dump get(struct map* map, struct mem_dump* key) { 
	int i;
	for (i = 0; i < map->size; i++) {
		if (!dump_cmp(&map->keys[i], key)) {
			// Key found, return the value
			return map->values[i];
		}
	}
	// Key not found
	return *dump_dummy();
}

void printMap(struct map* map) {

	int i;
	printf("{ ");
	for (i = 0; i < map->size; i++) {
		printf("%s: %d", map->keys[i], map->values[i]);
		if (i < map->size - 1) {
			printf(", ");
		}
	}
	printf(" }\n");
}

void map_delete(struct map* map, struct mem_dump* key) {
	int i;
	for (i = 0; i < map->size; i++) {
		if (!dump_cmp(&map->keys[i], key)) {
			// Key found, remove the key-value pair
			int j;
			for (j = i; j < map->size - 1; j++) {
				map->keys[j] = map->keys[j + 1];
				map->values[j] = map->values[j + 1];
			}
			map->size--;
			return;
		}
	}
	// Key not found
	printf("Key not found in the map.\n");
}

struct map* map_create() {
	int i;
	struct map* result_map = (void*)0;
	result_map = (struct map*)malloc(sizeof(struct map));
	result_map->size = 0;
	for (i = 0; i < MAX_MAP_SIZE; i++)
	{
		result_map->keys[i] = *dump_dummy();
		result_map->values[i] = *dump_dummy();
	}
	return result_map;
}