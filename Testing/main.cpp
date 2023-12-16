#include "gtest/gtest.h"
extern "C" 
{
#include "vconfig.h"
#include "string_generator.h"
#include "mem_dump.h"
#include "ibtp.h"
#include "ibtp_node.h"
#include <stdio.h>
#include <windows.h>
}

const char* btree_test_name = "test";
const wchar_t database_path[] = L"./test_database.txt";

void test_init();
void clear_database_state();
void create_and_pack(size_t key_length, size_t value_length, key* key, value* val);

//!< Storage.
static struct KV kv;
/*
* btree set of callback proxy functions
*/
btnode btree_page_get_proxy(btptr ptr) {
	return mdlIBTP.fileMappingRealization.treeCallbacks.page_get(&kv, ptr);
}
btptr btree_page_new_proxy(btnode node) {
	return mdlIBTP.fileMappingRealization.treeCallbacks.page_new(&kv, node);
}
void btree_page_del_proxy(btptr ptr) {
	mdlIBTP.fileMappingRealization.treeCallbacks.page_del(&kv, ptr);
}
/*
* free list set of callback proxy functions
*/
btnode free_list_page_get_proxy(btptr ptr) {
	return mdlIBTP.fileMappingRealization.freeListCallbacks.page_get(&kv, ptr);
}
btptr free_list_page_append_proxy(btnode node) {
	return mdlIBTP.fileMappingRealization.freeListCallbacks.page_append(&kv, node);
}
void free_list_page_use_proxy(btptr ptr, btnode node) {
	mdlIBTP.fileMappingRealization.freeListCallbacks.page_use(&kv, ptr, node);
}

TEST(TestCase, Test) {
	const size_t key_length = 1000;
	const size_t value_length = 3000;
	struct mem_dump packed_key; 
	struct mem_dump packed_value;
	btnode node; UINT16 bytes; UINT16 nkeys;
	int set_return_value;

	clear_database_state();

	mdlIBTP.fileMappingRealization.kvstore_open(&kv, btree_test_name, database_path, btree_page_get_proxy, btree_page_new_proxy, btree_page_del_proxy,
		free_list_page_get_proxy, free_list_page_append_proxy, free_list_page_use_proxy);

	create_and_pack(key_length, value_length, &packed_key, &packed_value);
	set_return_value = mdlIBTP.fileMappingRealization.main.set(&kv, &packed_key, &packed_value);
	EXPECT_EQ(set_return_value, 0);

	node = kv.tree.get(kv.tree.root);
	bytes = ibtp_nbytes(&node);
	nkeys = ibtp_nkeys(&node);

	EXPECT_EQ(bytes, 2 + 2 + (8 + 8) + (2 + 2) + ((2 + 2) + (0 + 0))+ ((2 + 2) + (key_length + value_length)));  //!< Consider zero key. @see ibtp_h.h
	EXPECT_EQ(nkeys, 2);

	create_and_pack(key_length, value_length, &packed_key, &packed_value);
	set_return_value = mdlIBTP.fileMappingRealization.main.set(&kv, &packed_key, &packed_value);

	node = kv.tree.get(kv.tree.root);
	bytes = ibtp_nbytes(&node);
	nkeys = ibtp_nkeys(&node);

	EXPECT_EQ(bytes, 2 + 2 + (8 + 8) + (2 + 2) + ((2 + 2) + (0 + 0)) + ((2 + 2) + (key_length + 0)));
	EXPECT_EQ(nkeys, 2);

	create_and_pack(key_length, value_length, &packed_key, &packed_value);
	set_return_value = mdlIBTP.fileMappingRealization.main.set(&kv, &packed_key, &packed_value);

	node = kv.tree.get(kv.tree.root);
	bytes = ibtp_nbytes(&node);
	nkeys = ibtp_nkeys(&node);

	EXPECT_EQ(bytes, 2 + 2 + (8 + 8 + 8) + (2 + 2 + 2) + ((2 + 2) + (0 + 0)) + ((2 + 2) + (key_length + 0)) + ((2 + 2) + (key_length + 0)));
	EXPECT_EQ(nkeys, 3);

	create_and_pack(key_length, value_length, &packed_key, &packed_value);
	set_return_value = mdlIBTP.fileMappingRealization.main.set(&kv, &packed_key, &packed_value);

	node = kv.tree.get(kv.tree.root);
	bytes = ibtp_nbytes(&node);
	nkeys = ibtp_nkeys(&node);

	EXPECT_EQ(bytes, 2 + 2 + (8 + 8 + 8 + 8) + (2 + 2 + 2 + 2) + ((2 + 2) + (0 + 0)) + ((2 + 2) + (key_length + 0)) + ((2 + 2) + (key_length + 0)) +
		((2 + 2) + (key_length + 0)));
	EXPECT_EQ(nkeys, 4);

	create_and_pack(1, 1, &packed_key, &packed_value);
	set_return_value = mdlIBTP.fileMappingRealization.main.set(&kv, &packed_key, &packed_value);

	node = kv.tree.get(kv.tree.root);
	bytes = ibtp_nbytes(&node);
	nkeys = ibtp_nkeys(&node);

	EXPECT_EQ(bytes, 2 + 2 + 8*4 + 2*4 + ((2 + 2) + (0 + 0)) + ((2 + 2) + (key_length + 0)) + ((2 + 2) + (key_length + 0)) +
		((2 + 2) + (key_length + 0)));
	EXPECT_EQ(nkeys, 4);

	mdlIBTP.fileMappingRealization.kvstore_close(&kv);

	dump_delete(&packed_key);
	dump_delete(&packed_value);
}

int main(int argc, char** argv) {
	test_init();
	srand(time(NULL));
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

void test_init() {
	SYSTEM_INFO sys_info = { 0 };
	GetSystemInfo(&sys_info);
	DWORD page_size = sys_info.dwPageSize;
	mdlIBTP.init(page_size);
}

void clear_database_state() {
	DWORD file_attr = GetFileAttributesW(database_path);
	if (file_attr != INVALID_FILE_ATTRIBUTES && !(file_attr & FILE_ATTRIBUTE_DIRECTORY))
	{
		if (_wremove(database_path) == 0)
			wprintf(L"Clear state of database[%ls]: finished with success.\n", database_path);
		else {
			wprintf(L"Clear state of database[%ls]: finished with error.\n", database_path);
			exit(1);
		}
	}
	else
		wprintf(L"Clear state of database[%ls]: the database file does not exist.\n", database_path);

}

void create_and_pack(size_t key_length, size_t value_length, key* key, value* val) {
	unsigned char* stringified_key = generate_string(key_length);
	unsigned char* stringified_value = generate_string(value_length);
	(*key) = __dump_pack(key_length, stringified_key);
	(*val) = __dump_pack(value_length, stringified_value);
}