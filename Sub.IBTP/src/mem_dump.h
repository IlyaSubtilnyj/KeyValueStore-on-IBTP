#ifndef MEM_DUMP_H
#define MEM_DUMP_H

#ifdef SUBIBTP_EXPORTS
#define IBTP_API __declspec(dllexport)
#else
#define IBTP_API __declspec(dllimport)
#endif // SUBIBTP_EXPORTS

struct mem_dump {
	size_t size;
	unsigned char* dump;
};

IBTP_API void dump_delete_fr(struct mem_dump* md);

size_t dump_get_size(struct mem_dump* md);

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

struct mem_dump* dump_set(struct mem_dump* md, size_t size, const unsigned char* dump, unsigned char replicate);

IBTP_API struct mem_dump* dump_pack(size_t size, const unsigned char* dump, unsigned char replicate);

int dump_cmp(struct mem_dump* first, struct mem_dump* second);

struct mem_dump dump_create_np_s(size_t requred_size);

struct mem_dump dump_create_np();

struct mem_dump* dump_dummy();

struct mem_dump __dump_pack(size_t size, const unsigned char* dump);

#endif