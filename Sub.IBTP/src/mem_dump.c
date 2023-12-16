#define MEMDUMPIMPL
#include "mem_dump.h"
#undef MEMDUMPIMPL

#include <stdlib.h>
#include <memory.h>

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
		return result;
	}
	result = dump_init(result);
	return result;
}

struct mem_dump* dump_create_s(size_t requred_size) {
	struct mem_dump* result;
	result = (struct mem_dump*)malloc(sizeof(struct mem_dump));
	if (result == (void*)0) {
		return result;
	}
	result->dump = (unsigned char*)malloc(requred_size);
	if (result->dump == (void*)0) {
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
struct mem_dump* dump_set(struct mem_dump* md, size_t size, const unsigned char* source, unsigned char replicate)
{
	if (md == (void*)0) return md;
	md->size = size;
	free(md->dump);
	if (replicate) {
		md->dump = (unsigned char*)malloc(size);
		if (md->dump == (void*)0) return md->dump;
	}
	md->dump = replicate ? (unsigned char*)memcpy((void*)md->dump, (const void*)source, size) : source;
	return md;
}

struct mem_dump* dump_pack(size_t size, const unsigned char* dump, unsigned char replicate) {
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

void* dump_get(struct mem_dump* md, int index, size_t element_size) {
	int offset = index * element_size;
	if (md->size <= offset) return (void*)0;
	return md->dump + offset;
}

struct mem_dump __dump_pack(size_t size, const unsigned char* dump) {
	struct mem_dump result_mem_dump;
	result_mem_dump.size = size;
	result_mem_dump.dump = dump;
	return result_mem_dump;
}