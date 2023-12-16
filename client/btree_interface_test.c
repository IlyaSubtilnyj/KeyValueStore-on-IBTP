#include <stdio.h>
#include <assert.h>
#define IBTP_NAMESPACE 1
#include "btree_interface_test.h"
#include "map.h"

/*! @file btree_interface_test.c
* 
*	@brief Stor.
* 
*	Keep nodes and kv-pairs in map that located in RAM.
*/
struct store 
{
	btree* tree;
	struct map* ref;
	struct map* pages;
};

/*! @file btree_interface_test.c
* 
*	@brief Store for testing, actually...
*/
static struct store map_store;

/*! @file btree_interfcae_test.c
* 
*	@brief Retrive and return node from RAM map.
*/
static btnode pageGet(btptr ptr){
	struct mem_dump* packed_ptr = dump_pack(sizeof(ptr), &ptr, 1);
	btnode node = get(map_store.pages, packed_ptr);
	dump_delete(packed_ptr);
	return node;
}
/*! @file btree_interface_test.c
* 
*	@brief Keep track of new B-tree node.
*/
static btptr newPage(btnode node) {
	assert(ibtp_nbytes(&node) <= BTREE_PAGE_SIZE); 
	btptr ptr = (btptr)node.dump;
	struct mem_dump* packed_ptr = dump_pack(sizeof(ptr), &ptr, 1);
	struct mem_dump cmp = get(map_store.pages, packed_ptr);
	assert(!dump_cmp(dump_dummy(), &cmp));
	insert(map_store.pages, packed_ptr, node);
	return ptr;
}

/*! @file btree_interface_test.c
* 
*	@brief Unregister node from map.
* 
*	This function not do not deallocate memory that is occupied by deleted node.
*/
static void pageDel(btptr ptr) {
	struct mem_dump* packed_ptr = dump_pack(sizeof(ptr), &ptr, 1);
	struct mem_dump cmp = get(map_store.pages, packed_ptr);
	assert(dump_cmp(dump_dummy(), &cmp) && "Incorrect node pointer while page deletion.");
	map_delete(map_store.pages, packed_ptr);
	dump_delete(packed_ptr);
}

/*! @file btree_interface_test.c
* 
*	@brief Function initialize/opens store.
* 
*	Creates new one without any nodes because it is located in RAM and doesn't store any ROM.
*/
struct store* open_store() {

	map_store.pages = map_create();
	map_store.ref = map_create();

	btree* tree = (btree*)malloc(sizeof(btree));
	tree ? tree->get = pageGet : 0;
	tree ? tree->allocate = newPage : 0;
	tree ? tree->del = pageDel : 0;
	tree->root = 0;
	tree->name = "test_tree";

	map_store.tree = tree;

	return &map_store;
}

/*! @defgroup test Main functions that are used in testing.
*
*	Contains main test functions especially that are used to interact with storage(B-tree structure).
*/

/*! @defgroup add_interface_test Add interfece method test functions.
*
*	Contains add test functions.
*/

/*! @defgroup delete_interface_test Delete interfece method test functions.
*
*	Contains delete test functions.
*/

/*! @file btree_interface_test.c
* 
*	@brief Method that adds kv-pair in storage(B-tree structure) and register kv-pair simultaniously.
* 
*	Add kv pair in map that located in RAM.
* 
*	@ingroup test Add test function.
*/
void add(struct store* manager, key* key, value* val) {
	Insert(manager->tree, key, val);
	insert(manager->ref, key, *val);
}

/*!	@file btree_interface_test.c
* 
*	@brief Method that deletes kv-pair by key in storage(B-tree structure) and unregister simultaniously.
* 
*	Delete kv-pair in map that located in RAM.
* 
*	@ingroup test Delete test function.
*/
boolean del(struct store* manager, key* key) {
	boolean result = Delete(manager->tree, key);
	map_delete(manager->ref, key);
	return result;
}

void givenIncorrectLengthKey_whenKeyIncorrect_thanRiseAssertFailure();

/*! @file btree_interaface_test.c
* 
*	@brief Series of add interface methods.
* 
*	It makes sence if tests are grouped in this way otherwise test can run independently.
* 
*	@ingroup add_interface_test Series of add tests.
*/
void add_test(struct store* p_map_store) {

	//craetes tree(acrually one root leaf node)
	add(p_map_store,
		dump_set(dump_create(), strlen("key"), "key", 1),
		dump_set(dump_create(), strlen("value"), "value", 1));

	//split root leaf node to many nodes(one internal node and two leaf nodes)
	add(p_map_store,
		dump_set(dump_create(), strlen("overflaw"), "overflaw", 1),
		dump_set(dump_create(), strlen("overflaw_value"), "overflaw_value", 1));

	//add key-value pair to the leaf node
	add(p_map_store,
		dump_set(dump_create(), strlen("na"), "na", 1),
		dump_set(dump_create(), strlen("na"), "na", 1));

	//replace key-value pair in the leaf node and extends page
	add(p_map_store,
		dump_set(dump_create(), strlen("na"), "na", 1),
		dump_set(dump_create(), strlen("new_super_long_value"), "new_super_long_value", 1));

	/*add(p_map_store,
		dump_set(dump_create(), strlen("root_split_long_fuck"), "root_split_long_fuck", 1),
		dump_set(dump_create(), strlen("root_split"), "root_split", 1));*/

	//givenIncorrectLengthKey_whenKeyIncorrect_thanRiseAssertFailure();

	del(p_map_store, dump_pack(strlen("key"), "key", 1));

	//del(p_map_store, dump_pack(strlen("root_split"), "root_split", 1));
	//del(p_map_store, dump_pack(strlen("overflaw"), "overflaw", 1));
	//del(p_map_store, dump_pack(strlen("root_split_lo"), "root_split_lo", 1));
	//simple append to leaf node test
	/*add(p_map_store,
		dump_set(dump_create(), strlen("p"), "p", 1),
		dump_set(dump_create(), strlen("p"), "p", 1));*/
}

/*! @file btree_interface_test.c
* 
*	@brief Bad test.
*
*	Key size should not exceed 1/4 of node size while value size 3/4 of node size
*	apart from expences on storage of header, pointers, offsers and key/value sizes.
*	So, if node size is 60 key size can't exceed 60 / 4 = 15 chars in length.
*
*	@error Key size can't so large relate to node size.
*	@ingroup add_interface_test Bad test.
*/
void givenIncorrectLengthKey_whenKeyIncorrect_thanRiseAssertFailure() {
	add(&map_store,
		dump_set(dump_create(), strlen("root_split_lo"), "root_split_lon", 1),
		dump_set(dump_create(), strlen("root_split_lo"), "root_split_lon", 1));
}

/*! @file btree_interface_test.c
*
*	@brief Tryting to delete border key.
*
*	Assertion failure indicates right behavior.
*
*	@error Assertion failure.
*	@ingroup delete_interface_test Bad test.
*/
void givenIncorrectKey_whenZeroBorderKey_thanRiseAssertionFailure() {
	del(&map_store, dump_dummy());
}

/*! @file btree_interface_test.c
* 
*	@brief Series of delete interface methods.
* 
*	It makes sence if tests are grouped in this way otherwise test can run independently.
* 
*	@ingroup delete_interface_test Series of delete tests.
*/
void delete_test(struct store* p_map_store) {

	//del(p_map_store, dump_pack(strlen("overflaw_key"), "overflaw_key", 1));

	del(p_map_store, dump_pack(strlen("root_split"), "root_split", 1));

	del(p_map_store, dump_pack(strlen("overflaw"), "overflaw", 1));

	del(p_map_store, dump_pack(strlen("overflaw"), "overflaw", 1));

	//givenIncorrectKey_whenZeroBorderKey_thanRiseAssertionFailure();
}

/*! file btree_intreface_test.c
* 
*	@brief Main test function that run series of add and delete interface methods.
* 
*	@ingroup test Main test function of functionality.
*/
int test(int argc, char** argv) {
	//mdlIBTP.test.init(60);
	struct store* p_map_store = open_store();
	add_test(p_map_store);
	delete_test(p_map_store);
	return 0;
}