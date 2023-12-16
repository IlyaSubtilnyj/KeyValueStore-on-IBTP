#ifndef IBTPH_H
#define IBTPH_H

#include <Windows.h>

#include "mem_dump.h" //!< Structure of nodes and key-value; implimintation of vector
#include "map.h"
#include "dynamic_array.h"

#ifdef SUBIBTP_EXPORTS
#define IBTP_API __declspec(dllexport)
#else
#define IBTP_API __declspec(dllimport)
#endif // SUBIBTP_EXPORTS

enum {
	BNODE_NODE = 1, //internal nodes without values
	BNODE_LEAF = 2	//leaf nodes with values
};

/*
| type | nkeys |  pointers  |   offsets  | key - values
|  2B  |  2B   | nkeys * 8B | nkeys * 2B | ...
*/

/*
| klen | vlen | key | val |
|  2B  |  2B  | ... | ... |
*/

extern IBTP_API const int BTREE_PAGE_SIZE; //page size

extern IBTP_API const int BTYPE_OFFSET; //size of type of node cell
extern IBTP_API const int NKEYS_OFFSET; //size of number of keys cell
extern IBTP_API const int POINTERS_OFFSET; //offset of list of pointers
//extern const int HEADER; //calculate like sum of type of node cell size and number of keys cell size

extern IBTP_API const int KLEN_OFFSET; //size of key length cell
extern IBTP_API const int VLEN_OFFSET; //size of value length cell
extern IBTP_API const int KV_OFFSET; //KAY-VALUE list of pairs offset
extern IBTP_API const int BTREE_MAX_KEY_SIZE; //limitation for key size
extern IBTP_API const int BTREE_MAX_VAL_SIZE; //limitation for value size

typedef DWORD64 ibtpptr; //references disk pages
typedef ibtpptr btptr;
typedef struct mem_dump btnode, key, value; //node and key-value

typedef struct BTree {
	char* name;
	btptr root;
	btnode (*get)(btptr); //dereference a pointer
	btptr(*allocate)(btnode); //allocate a new page
	void(*del)(btptr); /*! @brief Caller is responsible for deallocating of node that deleted; Receive node through get before delition and free resources. */
} btree;

typedef struct FreeList {
	btptr head;
	// callbacks for managing on-disk pages
	btnode (*get)(btptr); // dereference a pointer
	btptr (*append)(btnode); // append a new page
	void (*use)(btptr, btnode); // reuse a page
} FreeList;

/**
    @struct KV
    @brief  Structure that describes storage based on memory file mappings.
**/
struct KV {
	wchar_t* path; //!< Path of file od database.
	HANDLE hfile; //!< Handle of opened file of database.
	struct BTree tree; //!< Btree structure. Contains functions to manipulate storage.
	struct FreeList free; //!< Free list. List of pages that can be reused.
	 /**
		 @struct mmap
		 @brief  Keep info about map and view.
	 **/
	struct mmap {
		HANDLE oper_map; //!< Handle of operating Map of database.
		//DWORDLONG file; //!< Deprecated. Redundant. File size, can be larger than the database size.
		DWORDLONG total; //!< Map size and file size at the same time.
		unsigned char* chunk; //!< View of Map. Start address of view.
	} mmap;
	 /**
		 @struct page
		 @brief  Keep info about usage of pages and the pages themselves.
	 **/
	struct page {
		UINT64 flushed; //!< Database size in number of pages.
		int nfree; //!< Number of pages taken from the free list.
		int nappend; //!< Number of pages to be appended.
		struct map* updates; //!< Newly allocated or deallocated pages keyed by the pointer. "Deallocated" value denotes a deallocated page. Store pairs like [btptr, node memory start address].
		//struct mem_dump_node* temp; //!< Deprecated. Newly allocated pages. Were in use when there is no free list created.
	} page;
};

#endif // !IBTPH_H
