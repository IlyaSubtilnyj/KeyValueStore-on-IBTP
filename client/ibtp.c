#define IBTPIMPL
#include "ibtp.h"
#undef IBTPIMPL

#include <assert.h>

int immutable_b_tree_plus_init(DWORD page_size) {
	assert(page_size == BTREE_PAGE_SIZE);
	int node1max = POINTERS_OFFSET + 8 + 2 + 4 + BTREE_MAX_KEY_SIZE + BTREE_MAX_VAL_SIZE;
	assert(node1max <= BTREE_PAGE_SIZE);
	return 0;
}

int immutable_b_tree_plus_test_init(DWORD page_size) {
	int node1max = POINTERS_OFFSET + 8 + 2 + 4;
	assert(node1max <= page_size);
	BTREE_PAGE_SIZE = (int)page_size;
	return 0;
}

const struct _ibtp_namespace mdlIBTP = {
	.init = immutable_b_tree_plus_init,
	.fileMappingRealization = {
		.kvstore_open = kvstore_open,
		.kvstore_close = kvstore_close,
		.treeCallbacks = {
			.page_get = pageGet,
			.page_new = pageNew,
			.page_del = pageDel
		},
		.freeListCallbacks = {
			.page_get = pageGet,
			.page_append = pageAppend,
			.page_use = pageUse
		},
		.main = {
			.get = Get,
			.set = Set,
			.del = Del
		}
	}
};