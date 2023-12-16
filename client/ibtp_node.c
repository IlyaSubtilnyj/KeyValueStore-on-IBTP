#define IBTPNODEIMPL
#include "ibtp_node.h"
#undef IBTPNODEIMPL

#include <assert.h>
#include <stdio.h>

/*
* functions to work with node header
*/
UINT16 ibtp_btype(btnode* node) {
	UINT16 result = mdlUtils.binary.little_endian.uint16(node->dump + BTYPE_OFFSET);
	return result;
}
UINT16 ibtp_nkeys(btnode* node) {
	UINT16 result = mdlUtils.binary.little_endian.uint16(node->dump + NKEYS_OFFSET);
	return result;
}
void ibtp_set_header(btnode* node, UINT16 btype, UINT16 nkeys) {
	mdlUtils.binary.little_endian.put_uint16(node->dump + BTYPE_OFFSET, btype);
	mdlUtils.binary.little_endian.put_uint16(node->dump + NKEYS_OFFSET, nkeys);
}

/*
* functions to work with node pointers list
*/
btptr ibtp_get_ptr(btnode* node, UINT16 idx) {
	assert(idx < ibtp_nkeys(node));
	UINT16 pos = POINTERS_OFFSET + sizeof(btptr) * idx;
	return mdlUtils.binary.little_endian.uint64(node->dump + pos);
}
void ibtp_set_ptr(btnode* node, UINT16 idx, btptr value) {
	assert(idx < ibtp_nkeys(node));
	UINT16 pos = POINTERS_OFFSET + sizeof(btptr) * idx;
	mdlUtils.binary.little_endian.put_uint64(node->dump + pos, value);
}

/*
* functions to work with offset list
*/
UINT16 ibtp_offset_pos(btnode* node, UINT16 idx) {
	assert(1 <= idx && idx <= ibtp_nkeys(node));
	return POINTERS_OFFSET + sizeof(btptr) * ibtp_nkeys(node) + 2 * (idx - 1);
}
UINT16 ibtp_get_offset(btnode* node, UINT16 idx) {
	if (idx == 0) {
		return 0;
	}
	return mdlUtils.binary.little_endian.uint16(node->dump + ibtp_offset_pos(node, idx));
}
void ibtp_set_offset(btnode* node, UINT16 idx, UINT16 offset) {
	mdlUtils.binary.little_endian.put_uint16(node->dump + ibtp_offset_pos(node, idx), offset);
}

/*
* functions to work with key-value pairs
*/
UINT16 ibtp_kv_pos(btnode* node, UINT16 idx) {
	assert(idx <= ibtp_nkeys(node));
	return POINTERS_OFFSET + sizeof(btptr) * ibtp_nkeys(node) + 2 * ibtp_nkeys(node) + ibtp_get_offset(node, idx);
}
key* ibtp_get_key(btnode* node, UINT16 idx, boolean replicate) {
	assert(idx < ibtp_nkeys(node));
	UINT16 pos = ibtp_kv_pos(node, idx);
	UINT16 klen = mdlUtils.binary.little_endian.uint16(node->dump + pos + KLEN_OFFSET);
	return dump_set(dump_create(), klen, node->dump + pos + KV_OFFSET, replicate);
}
value* ibtp_get_val(btnode* node, UINT16 idx, boolean replicate) {
	assert(idx < ibtp_nkeys(node));
	UINT16 pos = ibtp_kv_pos(node, idx);
	UINT16 klen = mdlUtils.binary.little_endian.uint16(node->dump + pos + KLEN_OFFSET);
	UINT16 vlen = mdlUtils.binary.little_endian.uint16(node->dump + pos + VLEN_OFFSET);
	return dump_set(dump_create(), vlen, node->dump + pos + KV_OFFSET + klen, replicate);
}

/*
* function to get size of the node
*/
UINT16 ibtp_nbytes(btnode* node) {
	return ibtp_kv_pos(node, ibtp_nkeys(node));
}

/*
* function that looks up the key in node and 
* returns the index of the first kid node whose range intersects the key (kid[i] <= key);
* works for both leaf and internal nodes
*/
UINT16 ibtp_node_lookup(btnode* node, key* search_key) {
	UINT16 nkeys = ibtp_nkeys(node);
	UINT16 found = 0;
	// the first key is a copy from the parent node,
	// thus it's always less than or equal to the key.
	int first = 1;
	int last = nkeys - 1;
	while (first <= last) {
		int middle = (first + last) / 2;
		key* oper_key = ibtp_get_key(node, middle, 0);
		int cmp = dump_cmp(oper_key, search_key);
		free(oper_key);
		if (cmp < 0) {
			found = middle;
			first = middle + 1;
		}
		else if (cmp > 0) {
			last = middle - 1;
		}
		else {
			found = middle;
			break;
		}
	}
	return found;
}

/*
* update leaf node(copies content to new) and add a new key to a leaf node
*/
void ibtp_leaf_insert(btnode* newest, btnode* old, UINT16 idx, key* key, value* val) {
	ibtp_set_header(newest, BNODE_LEAF, ibtp_nkeys(old) + 1);
	ibtp_node_append_range(newest, old, 0, 0, idx);
	ibtp_node_append_kv(newest, idx, 0, key, val);
	ibtp_node_append_range(newest, old, idx + 1, idx, ibtp_nkeys(old) - idx);
}

/*
* update leaf node(copies content to new) and edit the key in a leaf node
*/
void ibtp_leaf_update(btnode* newest, btnode* old, UINT16 idx, key* key, value* val) {
	ibtp_set_header(newest, BNODE_LEAF, ibtp_nkeys(old));
	ibtp_node_append_range(newest, old, 0, 0, idx);
	ibtp_node_append_kv(newest, idx, 0, key, val);
	if (ibtp_nkeys(old) - 1 - idx > 0)
		ibtp_node_append_range(newest, old, idx + 1, idx + 1, ibtp_nkeys(old) - 1 - idx);
}

/*
* function copies keys from an old to new node; copies multiple KVs into the position
*/
void ibtp_node_append_range(btnode* newest, btnode* old, UINT16 dstNew, UINT16 srcOld, UINT16 n) {
	assert(srcOld + n <= ibtp_nkeys(old));
	assert(dstNew + n <= ibtp_nkeys(newest));
	if (n == 0) {
		return;
	}
	/*
	* copy pointers
	*/
	for (int i = 0; i < n; i++) {
		ibtp_set_ptr(newest, dstNew + i, ibtp_get_ptr(old, srcOld + i));
	}
	/*
	* copy offsets
	*/
	UINT16 dstBegin = ibtp_get_offset(newest, dstNew);
	UINT16 srcBegin = ibtp_get_offset(old, srcOld);
	for (int i = 1; i <= n; i++) { // NOTE: the range is [1, n]
		UINT16 offset = dstBegin + ibtp_get_offset(old, srcOld + i) - srcBegin;
		ibtp_set_offset(newest, dstNew + i, offset);
	}
	/*
	* copy key-value pairs
	*/
	UINT16 begin = ibtp_kv_pos(old, srcOld);
	UINT16 end = ibtp_kv_pos(old, srcOld + n);
	memcpy(newest->dump + ibtp_kv_pos(newest, dstNew), old->dump + begin, end - begin);
}

/*
* functions copies a KV into the position
*/
void ibtp_node_append_kv(btnode* newest, UINT16 idx, UINT64 ptr, key* key, value* val) {
	/*
	* ptrs
	*/
	ibtp_set_ptr(newest, idx, ptr);
	/*
	* KVs
	*/
	UINT16 pos = ibtp_kv_pos(newest, idx);
	mdlUtils.binary.little_endian.put_uint16(newest->dump + pos + KLEN_OFFSET, dump_get_size(key));
	mdlUtils.binary.little_endian.put_uint16(newest->dump + pos + VLEN_OFFSET, dump_get_size(val));
	memcpy(newest->dump + pos + KV_OFFSET, key->dump, dump_get_size(key));
	memcpy(newest->dump + pos + KV_OFFSET + dump_get_size(key), val->dump, dump_get_size(val));
	/*
	* the offset of the next key
	*/
	ibtp_set_offset(newest, idx + 1, ibtp_get_offset(newest, idx) + KV_OFFSET + (key->size + val->size));
}

void translate(btnode* node) {
	printf("Type:%d;Nkeys:%d;", ibtp_btype(node), ibtp_nkeys(node));

	for (int i = 0; i < ibtp_nkeys(node); i++)
	{
		printf("Key:");
		key* k = ibtp_get_key(node, i, 0);
		for (int j = 0; j < k->size; j++)
		{
			printf("%c", *(k->dump + j));
		}
	}
}
