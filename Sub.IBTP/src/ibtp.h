#ifndef IBPT_H
#define IBPT_H

#include "ibtp_h.h"

#ifdef SUBIBTP_EXPORTS
#define IBTP_API __declspec(dllexport)
#else
#define IBTP_API __declspec(dllimport)
#endif // SUBIBTP_EXPORTS

/**
	@struct _ibtp_namespace
	@brief  Btree nothing knows about how pages will be stored(storage type), so each realization implement page manupilation functions based on storage type
			(realization of btree is already implemented and each realization must use it). So you need to set Btree callbacks functions and FreeList callback functions
			to btree can operate your storage(so btree implementation nothing knows about storage type). So each implementation provide functions like `*store_open`
			and `*store_close`.
**/
struct _ibtp_namespace {
	struct {
		int (*kvstore_open)(struct KV* db, const char* name, const wchar_t* path,
			btnode(*btree_get_callback)(btptr ptr),
			btptr(*btree_new_callback)(btnode node),
			void (*btree_del_callback)(btptr ptr),
			btnode(*free_list_get_callback)(btptr ptr),
			btptr(*free_list_append_callback)(btnode node),
			void (*free_list_use_callback)(btptr ptr, btnode node));
		void (*kvstore_close)(struct KV* db);
		struct {
			btnode(*page_get)(struct KV* kv, btptr ptr);
			btptr(*page_new)(struct KV* db, btnode node);
			void (*page_del)(struct KV* db, btptr ptr);
		} treeCallbacks;
		struct {
			btnode(*page_get)(struct KV* kv, btptr ptr);
			btptr(*page_append)(struct KV* kv, btnode node);
			void (*page_use)(struct KV* db, btptr ptr, btnode node);
		} freeListCallbacks;
		struct {
			value* (*get)(struct KV* db, key* key);
			int (*set)(struct KV* stg, key* key, value* val);
			int (*del)(struct KV* stg, key* key);
		} main;
		struct {
			void (*dump_tree)(FILE* file, btree* tree, btnode* node, int level);
			void (*dump_node)(FILE* file, btnode* node, int level);
		} utils;
	} fileMappingRealization;
};

/**
	@brief  Library initialization function.
	@param[in]  page_size - Size of memory page on your system. Call GetSystemInfo function and firld dwPageSize in your interest to pass.
	@retval				  - Zero indicates success. If parameter page_size is not equal to BTREE_PAGE_SIZE returned 1. Other returned value
	indicates inner library corruption.
*/
int IBTP_API ImmutableBTreePlus_initialize(DWORD page_size);

const extern IBTP_API struct _ibtp_namespace mdlIBTP; //!< Module containing functions of file mapping realization.


#endif