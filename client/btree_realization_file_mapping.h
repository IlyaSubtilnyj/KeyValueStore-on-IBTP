#ifndef BTREEREALIZATIONFILEMAPPING_H
#define BTREEREALIZATIONFILEMAPPING_H

#include "ibtp_h.h"

int mmap_init(HANDLE mapping_file, HANDLE* file_mapping_handle, DWORDLONG* file_mapping_size, unsigned char** start_virtual_address);
int extend(struct KV* kv, int npages);
int masterLoad(struct KV* kv);
int masterStore(struct KV* kv);
int writePages(struct KV* db);
int syncPages(struct KV* db);
int flushPages(struct KV* db);

/*! Function to initialize storage. Must be provided by each implementation. */
void kvstore_close(struct KV* db);
int kvstore_open(struct KV* db, const char* name, const wchar_t* path, 
	btnode (*btree_get_callback)(btptr ptr),
	btptr (*btree_new_callback)(btnode node),
	void (*btree_del_callback)(btptr ptr),
	btnode (*free_list_get_callback)(btptr ptr),
	btptr(*free_list_append_callback)(btnode node),
	void (*free_list_use_callback)(btptr ptr, btnode node));

/*! btree callback function implementation*/
btnode pageGetMapped(struct KV* kv, btptr ptr/*number of page*/);
btnode pageGet(struct KV* kv, btptr ptr);
btptr pageNew(struct KV* db, btnode node);
void pageDel(struct KV* db, btptr ptr);

/*! free list callback functions implementation*/
// callback for FreeList, allocate a new page.
btptr pageAppend(struct KV* kv, btnode node);
// callback for FreeList, reuse a page.
void pageUse(struct KV* db, btptr ptr, btnode node);

value* Get(struct KV* db, key* key);
int Set(struct KV* stg, key* key, value* val);
int Del(struct KV* stg, key* key);

#endif // !BTREEREALIZATIONFILEMAPPING_H