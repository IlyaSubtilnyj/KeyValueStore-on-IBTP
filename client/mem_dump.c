#define MEMDUMPIMPL
#include "mem_dump.h"
#undef MEMDUMPIMPL

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <malloc.h>
#include <assert.h>

size_t dump_get_size(struct mem_dump* md) {
	return md->size;
}

void dump_delete(struct mem_dump* md) {
	free(md->dump);
	md->size = 0;
}

/**
    @brief Dump is undefined after call. Caller should not use it.
    @param[in] Dump of memory that caller get from ibtp library function. 
**/
void dump_delete_fr(struct mem_dump* md) {
	free(md->dump);
	free(md);
}

struct mem_dump* dump_init(struct mem_dump* md) {
	md->size = 0;
	md->dump = (unsigned char*)0;
	return md;
}

/*! @file mem_dump
*	@brief Creates dump of memory on heap; Space for inner pointer not allocated. 
* 
*	@return Pointer to allocated dump of memory.
*/
struct mem_dump* dump_create() {
	struct mem_dump* result;
	result = (struct mem_dump*)malloc(sizeof(struct mem_dump));
	if (result == (void*)0) {
		printf("Unsufficint memory available\n");
		return result;
	}
	result = dump_init(result);
	return result;
}

struct mem_dump* dump_create_s(size_t requred_size) {
	struct mem_dump* result;
	result = (struct mem_dump*)malloc(sizeof(struct mem_dump));
	if (result == (void*)0) {
		printf("Unsufficint memory available\n");
		return result;
	}
	result->dump = (unsigned char*)malloc(requred_size);
	if (result->dump == (void*)0) {
		printf("Unsufficint memory available\n");
		return result;
	}
	result->size = requred_size;
	return result;
}

struct mem_dump dump_create_np() {
	return *dump_create();
}

struct mem_dump dump_create_np_s(size_t requred_size) {
	return *dump_create_s(requred_size);
}

static struct mem_dump dummy;

struct mem_dump* dump_dummy() {
	dump_init(&dummy);
	return &dummy;
}


/*! @file mem_dump
*	@brief Sets value of mem_dump structure.
*	
*	@param[in] replicate If !=0 - allocate space on heap and copy memory from source; if =0 - copy pointer on source.
* 
*	@return Pointer to md to be able create chain of functions.
*/
struct mem_dump* dump_set(struct mem_dump* md, size_t size, const unsigned char* source, boolean replicate) 
{
	assert(md);
	md->size = size;
	free(md->dump);
	if (replicate) {
		md->dump = (unsigned char*)malloc(size);
		assert(md->dump);
	}
	md->dump = replicate ? (unsigned char*)memcpy((void*)md->dump, (const void*)source, size) : source;
	return md;
}

struct mem_dump* dump_pack(size_t size, const unsigned char* dump, boolean replicate) {
	struct mem_dump* result = dump_create();
	dump_set(result, size, dump, replicate);
	return result;
}

int dump_cmp(struct mem_dump* first, struct mem_dump* second) {
	size_t size_of_first = dump_get_size(first);
	size_t size_of_second = dump_get_size(second);

	size_t cmp_size = size_of_first < size_of_second ? size_of_first : size_of_second;

	int cmp_result = memcmp(first->dump, second->dump, cmp_size);

	if (cmp_result == 0) {
		if (size_of_first == size_of_second) return cmp_result;
		if (size_of_first < size_of_second) return -1;
		if (size_of_first > size_of_second) return 1;
	}
	else return cmp_result;
}

/*! @file mem_dump
*
*	@brief Deprecated.
*
*	The problem with which it was created was solved in a different way.
*/
int dump_get_count(struct mem_dump* md, size_t element_size) {
	assert((md->size % element_size == 0) && "type mismatch");
	return (md->size / element_size);
}

void* dump_get(struct mem_dump* md, int index, size_t element_size) {
	int offset = index * element_size;
	assert(md->size > offset);
	return md->dump + offset;
}

/*! @file mem_dump
*
*	@brief Deprecated.
*
*	The purpose with which it was created was solved in a different way.
*/
void dump_append(struct mem_dump* md, unsigned char* element, size_t element_size) {

	size_t new_size;
	unsigned char* new_dump;

	if (element_size == 0) return;

	new_size = md->size + element_size;
	if ((new_dump = (unsigned char*)realloc(md->dump, new_size)) == (void*)0) {
		printf("realloc failure\n");
		return;
	}
	new_size = _msize(new_dump);

	memcpy(new_dump + md->size, element, element_size);

	md->size = new_size;
	md->dump = new_dump;
}

struct mem_dump __dump_pack(size_t size, const unsigned char* dump) {
	struct mem_dump result_mem_dump;
	result_mem_dump.size = size;
	result_mem_dump.dump = dump;
	return result_mem_dump;
}