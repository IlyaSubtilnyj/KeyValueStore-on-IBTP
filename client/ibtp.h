#ifndef IBPT_H
#define IBPT_H

#include <windows.h>

#include "ibtp_h.h"

//Btree nothing knows about how pages will be stored(storage type), so each realization implement page manupilation functions based on storage type 
//(realization of btree is already implemented and each realization must use it). So you need to set Btree callbacks functions and FreeList callback functions
//to btree can operate your storage(so btree implementation nothind knows about storage type). So each implementation provide functions like `*store_open`
//and `*store_close`.
struct _ibtp_namespace {
	int (*init)(DWORD page_size);
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
			btptr (*page_new)(struct KV* db, btnode node);
			void (*page_del)(struct KV* db, btptr ptr);
		} treeCallbacks;
		struct {
			btnode (*page_get)(struct KV* kv, btptr ptr);
			btptr (*page_append)(struct KV* kv, btnode node);
			void (*page_use)(struct KV* db, btptr ptr, btnode node);
		} freeListCallbacks;
		struct {
			value* (*get)(struct KV* db, key* key);
			int (*set)(struct KV* stg, key* key, value* val);
			int (*del)(struct KV* stg, key* key);
		} main;
	} fileMappingRealization;
};

#if IBTP_NAMESPACE == IBTP_NAMESPACE_DISABLED
#include "btree_realization_file_mapping.h"
#else
const extern struct _ibtp_namespace mdlIBTP;
#endif // NETIMPL

#endif