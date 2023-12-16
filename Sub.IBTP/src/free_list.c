#include "../pch.h"
#define FREELISTIMPL
#include "free_list.h"
#include "utils.h"
#undef FREELISTIMPL

#include "ibtp_h.h"

#include <assert.h>

/*! @brief Free list realization! */
/*
| type | size | total | next | pointers  |
|  2B  |  2B  |   8B  |   8B | size * 8B |
*/

const BNODE_FREE_LIST = 3; //Node type
const FREE_LIST_HEADER = 2 + 2 + 8 + 8;
const FREE_LIST_CAP = (4096 - 2 - 2 - 8 - 8) / 8;// Capacity of free list node. (BTREE_PAGE_SIZE - FREE_LIST_HEADER) / 8;

int flnSize(btnode node) {
	assert(mdlUtils.binary.little_endian.uint16(node.dump) == BNODE_FREE_LIST);
	return mdlUtils.binary.little_endian.uint16(node.dump + 2);
}
btptr flnNext(btnode node) {
	assert(mdlUtils.binary.little_endian.uint16(node.dump) == BNODE_FREE_LIST);
	return mdlUtils.binary.little_endian.uint64(node.dump + 2 + 2 + 8);
}
btptr flnPtr(btnode node, int idx) {
	assert(mdlUtils.binary.little_endian.uint16(node.dump) == BNODE_FREE_LIST);
	return mdlUtils.binary.little_endian.uint64(node.dump + FREE_LIST_HEADER + idx * sizeof(btptr));
}
void flnSetPtr(btnode node, int idx, btptr ptr) {
	assert(mdlUtils.binary.little_endian.uint16(node.dump) == BNODE_FREE_LIST);
	mdlUtils.binary.little_endian.put_uint64(node.dump + FREE_LIST_HEADER + idx * sizeof(btptr), ptr);
}
void flnSetHeader(btnode node, int size, btptr next) {
	mdlUtils.binary.little_endian.put_uint16(node.dump, BNODE_FREE_LIST);
	mdlUtils.binary.little_endian.put_uint16(node.dump + 2, size);
	mdlUtils.binary.little_endian.put_uint64(node.dump + 2 + 2 + 8, next);
}
void flnSetTotal(btnode node, int total) {
	assert(mdlUtils.binary.little_endian.uint16(node.dump) == BNODE_FREE_LIST);
	mdlUtils.binary.little_endian.put_uint64(node.dump + 2 + 2, total);
}

int Total(FreeList* fl) {
	int result = 0;
	btptr head = fl->head;
	while (head != 0) {
		btnode node = fl->get(head);
		result += flnSize(node);
		head = flnNext(node);
	}
	flnSetTotal(fl->get(fl->head), result);
	return result;
}

/*! @brief Getting the topn'th item from the list. */
btptr flGet(FreeList* fl, int topn) {
	assert((0 <= topn) && (topn < Total(fl)));
	btnode node = fl->get(fl->head);
	for (; flnSize(node) <= topn;) {
		topn -= flnSize(node);
		btptr next = flnNext(node);
		assert(next != 0);
		node = fl->get(next);
	}
	return flnPtr(node, flnSize(node) - topn - 1);
}

void flPush(FreeList* fl, struct dynamic_array* freed, struct dynamic_array* reuse) {
	for (; freed->size > 0;) {
		btnode new = dump_create_np_s(BTREE_PAGE_SIZE);
		// construct a new node
		int size = freed->size;
		if (size > FREE_LIST_CAP) {
			size = FREE_LIST_CAP;
		}
		flnSetHeader(new, size, fl->head);
		for (int i = 0; i < size; i++)
		{
			btptr ptr = *((btptr*)da_get(freed, i));
			flnSetPtr(new, i, ptr);
		}
		da_delete_first_n(freed, size);
		if (reuse->size > 0) {
			// reuse a pointer from the list
			fl->head = *((btptr*)da_get(reuse, 0));
			da_delete_first_n(reuse, 1);
			fl->use(fl->head, new);
		}
		else {
			// or append a page to house the new node
			fl->head = fl->append(new);
		}
	}
	assert(reuse->size == 0);
}

void Update(FreeList* fl, int popn, struct dynamic_array* freed) {

	assert(popn <= Total(fl));
	if ((popn == 0) && (freed->size == 0)) {
		return; // nothing to do
	}
	// prepare to construct the new list
	int total = Total(fl);
	struct dynamic_array reuse = da_create(sizeof(btptr));

	for (; (fl->head != 0) && (reuse.size * FREE_LIST_CAP < freed->size);)
	{
		btnode node = fl->get(fl->head);
		da_insert(freed, &fl->head, sizeof(btptr)); // recyle the node itself
		if (popn >= flnSize(node)) {
			// phase 1
			// remove all pointers in this node
			popn -= flnSize(node);
		}
		else {
			// phase 2:
			// remove some pointers
			int remain = flnSize(node) - popn;
			popn = 0;
			// reuse pointers from the free list itself
			for (; (remain > 0) && (reuse.size * FREE_LIST_CAP < freed->size + remain);) {
				remain--;
				btptr new = flnPtr(node, remain);
				da_insert(&reuse, &new, sizeof(btptr));
			}
			// move the node into the `freed` list
			for (int i = 0; i < remain; i++) {
				btptr new = flnPtr(node, i);
				da_insert(freed, &new, sizeof(btptr));
			}
		}
		// discard the node and move to the next node
		total -= flnSize(node);
		fl->head = flnNext(node);
	}
	assert((reuse.size * FREE_LIST_CAP >= freed->size) || (fl->head == 0));
	// phase 3: prepend new nodes
	flPush(fl, freed, &reuse);
	// done
	flnSetTotal(fl->get(fl->head), total + freed->size);

	da_clear(&reuse);
}