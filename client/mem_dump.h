#ifndef MEM_DUMP_H
#define MEM_DUMP_H

#include <windows.h>

struct mem_dump {
	size_t size;
	unsigned char* dump;
};

void dump_delete_fr(struct mem_dump* md);

size_t dump_get_size(struct mem_dump* md);

int dump_get_count(struct mem_dump* md, size_t element_size);

/*! @file mem_dump
*
*	@brief Deprecated.
*
*	The purpose with which it was created was solved in a different way.
*/
void* dump_get(struct mem_dump* md, int index, size_t element_size);

void dump_delete(struct mem_dump* md);

struct mem_dump* dump_init(struct mem_dump* md);

struct mem_dump* dump_create();

struct mem_dump* dump_create_s(size_t requred_size);

struct mem_dump* dump_set(struct mem_dump* md, size_t size, const unsigned char* dump, boolean replicate);

struct mem_dump* dump_pack(size_t size, const unsigned char* dump, boolean replicate);

void dump_append(struct mem_dump* md, unsigned char* element, size_t element_size);

int dump_cmp(struct mem_dump* first, struct mem_dump* second);

struct mem_dump dump_create_np_s(size_t requred_size);

struct mem_dump dump_create_np();

struct mem_dump* dump_dummy();

struct mem_dump __dump_pack(size_t size, const unsigned char* dump);

#endif