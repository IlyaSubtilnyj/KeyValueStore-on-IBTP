#define IBTPIMPL
#include "ibtp.h"
#include "ibtp_tree.h"
#include "btree_realization_file_mapping.h"
#undef IBTPIMPL

int ImmutableBTreePlus_initialize(DWORD page_size) {
	if (page_size != BTREE_PAGE_SIZE) return 1;
	const int _node_max = POINTERS_OFFSET + 8 + 2 + 4 + BTREE_MAX_KEY_SIZE + BTREE_MAX_VAL_SIZE;
	if (_node_max > BTREE_PAGE_SIZE) return 2;
	if (
		(mdlIBTP.fileMappingRealization.kvstore_open != &kvstore_open) ||
		(mdlIBTP.fileMappingRealization.kvstore_close != &kvstore_close) ||
		(mdlIBTP.fileMappingRealization.treeCallbacks.page_get != &pageGet) ||
		(mdlIBTP.fileMappingRealization.treeCallbacks.page_new != &pageNew) ||
		(mdlIBTP.fileMappingRealization.treeCallbacks.page_del != &pageDel) ||
		(mdlIBTP.fileMappingRealization.freeListCallbacks.page_get != &pageGet) ||
		(mdlIBTP.fileMappingRealization.freeListCallbacks.page_append != &pageAppend) ||
		(mdlIBTP.fileMappingRealization.freeListCallbacks.page_use != &pageUse) ||
		(mdlIBTP.fileMappingRealization.main.get != &Get) ||
		(mdlIBTP.fileMappingRealization.main.set != &Set) ||
		(mdlIBTP.fileMappingRealization.main.del != &Del) ||
		(mdlIBTP.fileMappingRealization.utils.dump_node != &ibtp_print_node) ||
		(mdlIBTP.fileMappingRealization.utils.dump_tree != &ibtp_print_tree)
		) return 3;
	return 0;
}

const struct _ibtp_namespace mdlIBTP = {
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
		},
		.utils = {
			.dump_tree = ibtp_print_tree,
			.dump_node = ibtp_print_node
		}
	}
};