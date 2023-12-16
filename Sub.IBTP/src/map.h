#pragma once
#include <stdio.h>
#include <assert.h>
#include "mem_dump.h"

#define MAX_MAP_SIZE 1000

/*! @file btree_interfac_test.c
*
*	@brief Custom realization of map.
*
*	Operates dumps of memory structures. @see @ref mem_dump.h
* 
*/
struct map
{
	int size;								// Current number of elements in the map
	struct mem_dump keys[MAX_MAP_SIZE];		// Array to store the keys
	struct mem_dump values[MAX_MAP_SIZE];	// Array to store the values
};
/*! @file btree_interface_test.c
*
*	@brief Function to insert a key-value pair into the map
*/
void insert(struct map* map, struct mem_dump* key, struct mem_dump val);

/*! @file btree_interface_test.c
*
*	@brief Function to get the value of a key in the map.
* 
*	If value not found return dummy node.
*/
struct mem_dump get(struct map* map, struct mem_dump* key);

/*! @file btree_interface_test.c
*
*	@brief Function to print the map.
*/
void printMap(struct map* map);

/*! @file btree_interface_test.c
*
*	@brief Function to delete a key-value pair from the map.
*/
void map_delete(struct map* map, struct mem_dump* key);

/*! @file btree_interface_test.c
*
*	@brief Function creates and initializes map.
*/
struct map* map_create();

void clear(struct map* map);