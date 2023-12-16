#include <windows.h>

#include "vconfig.h"
#include "net.h"
#include "ibtp.h"
#include "logger/logger.h"

#pragma comment( compiler )
#pragma comment( user, "Compiled on " __DATE__ " at " __TIME__ )

//!< Storage.
static struct KV kv;
/*
* btree set of callback proxy functions
*/
btnode btree_page_get_proxy(btptr ptr) {
	LOG_TRACEF("args: ptr[%d]", ptr);
	return mdlIBTP.fileMappingRealization.treeCallbacks.page_get(&kv, ptr);
}
btptr btree_page_new_proxy(btnode node) {
	LOG_TRACEF("args: node[.size[%d], .dump[%p]]", node.size, node.dump);
	return mdlIBTP.fileMappingRealization.treeCallbacks.page_new(&kv, node);
}
void btree_page_del_proxy(btptr ptr) {
	LOG_TRACEF("args: ptr[%d]", ptr);
	mdlIBTP.fileMappingRealization.treeCallbacks.page_del(&kv, ptr);
}
/*
* free list set of callback proxy functions
*/
btnode free_list_page_get_proxy(btptr ptr) {
	LOG_TRACEF("args: ptr[%d]", ptr);
	return mdlIBTP.fileMappingRealization.freeListCallbacks.page_get(&kv, ptr);
}
btptr free_list_page_append_proxy(btnode node) {
	LOG_TRACEF("args: node[.size[%d], .dump[%p]]", node.size, node.dump);
	return mdlIBTP.fileMappingRealization.freeListCallbacks.page_append(&kv, node);
}
void free_list_page_use_proxy(btptr ptr, btnode node) {
	LOG_TRACEF("args: ptr[%d], node[.size[%d], .dump[%p]]", ptr, node.size, node.dump);
	mdlIBTP.fileMappingRealization.freeListCallbacks.page_use(&kv, ptr, node);
}

static int status;
int main(int argc, char* argv[])
{
#if 1
	char* data_string = "data_string";
	char data_string_in_massive[100] = "data_string";
	char* data_string_second_pointer = "data_string";
	printf("first_pointer[%p]; massive[%p]; second_pointer[%p]; natural_string[%p]",
		data_string, data_string_in_massive, data_string_second_pointer, "data_string");

	status = mdlNet.lib.start(MAKEWORD(2, 2));
	if (status) return status;

	//ibtp_init();
	SYSTEM_INFO sys_info = { 0 };
	GetSystemInfo(&sys_info);
	DWORD page_size = sys_info.dwPageSize;

	status = mdlIBTP.init(page_size);
	if (status) return status;

	mdlIBTP.fileMappingRealization.kvstore_open(&kv, "Nice try", L"default.db.txt", btree_page_get_proxy, btree_page_new_proxy, btree_page_del_proxy,
		free_list_page_get_proxy, free_list_page_append_proxy, free_list_page_use_proxy);

	key* key = dump_pack(strlen("key"), "key", 1);
	value* value1 = dump_pack(strlen("value"), "value", 1);
	mdlIBTP.fileMappingRealization.main.set(&kv, key, value1);

	value* val = mdlIBTP.fileMappingRealization.main.get(&kv, key);
	printf("%.*s\n", (unsigned int)val->size, val->dump);
	dump_delete_fr(val);

	struct mem_dump* key2 = dump_pack(strlen("key2"), "key2", 1);
	struct mem_dump* value2 = dump_pack(strlen("value2"), "value2", 1);
	mdlIBTP.fileMappingRealization.main.set(&kv, key2, value2);

	struct mem_dump* key3 = dump_pack(strlen("key69"), "key69", 1);
	struct mem_dump* value3 = dump_pack(strlen("key69"), "key69", 1);
	mdlIBTP.fileMappingRealization.main.set(&kv, key3, value3);

	mdlIBTP.fileMappingRealization.main.del(&kv, key);
	mdlIBTP.fileMappingRealization.main.del(&kv, key3);

	mdlIBTP.fileMappingRealization.kvstore_close(&kv);

	status = mdlNet.lib.clear();
	return status;
#endif
}

