#define BTREEREALIZATIONFILEMAPPINGIMPL
#include "btree_realization_file_mapping.h"
#include "utils.h"
#undef BTREEREALIZATIONFILEMAPPINGIMPL

#include "ibtp_tree.h"
#include "free_list.h"
#include <assert.h>

/**
	@brief  Function creates the initial mmap that covers the whole file.
	Function gets handle to the desired mapping file which must be opened before calling this function otherwise error is raised.
	All following parameters are pointers to memory where returned values will be written.
	Caller must memorize and close handles returned by this functions, they are two - file_mapping_handle(CloseHandle must be called) and address(UnmapViewOfFile mbc.).
	Returned value is 0 and mapping_file_size is 0 indicates that mapping was not created cause
	@param[in] mapping_file - File which will be mapped.
	@param[out]	file_mapping_handle	- Handle to file mapping kernel object.
	@param[out]	file_mapping_size	- Size of file mapping.
	@param[out]	start_virtual_address - Address of View kernel object.
	@retval	error - Function error code. 0 - Success. 1 - File handle is invalid. 2 - File size is not multiplicant of page size.
	3 - File mapping creation failed. If returned value is 3 and file_mapping_handle and file_mapping_size are both 0 then file size is 0 and
	map will be created at first write. 4 - View creation failed.
	Extended error code can be retrieved by syscalling GetLastError() fanction.
**/
int mmap_init(HANDLE mapping_file, HANDLE* file_mapping_handle, DWORDLONG* file_mapping_size, unsigned char** start_virtual_address) {
	BY_HANDLE_FILE_INFORMATION file_info;
	if (!GetFileInformationByHandle(mapping_file, &file_info))
		return 1;
	*file_mapping_size = file_info.nFileSizeLow;

	assert((*file_mapping_size % BTREE_PAGE_SIZE == 0) && "File size is not a multiple of page size.");
	if ((*file_mapping_size) % BTREE_PAGE_SIZE != 0) {
		return 2;
	}

	if ((*file_mapping_handle =
		CreateFileMappingW(mapping_file, NULL, PAGE_READWRITE,
			0, *file_mapping_size, NULL)) == NULL) {
		return 3;
	}
	if ((*start_virtual_address = (unsigned char*)MapViewOfFile(*file_mapping_handle, FILE_MAP_ALL_ACCESS,
		0, 0, 0)) == NULL) {
		CloseHandle(*file_mapping_handle);
		return 4;
	}
	return 0;
}

/**
	@brief  Extends database file and file mapping.
	@param  kv     - Database (KV store).
	@param  npages - Count of pages that database must store.
	@retval        - Error code.
**/
int extend(struct KV* kv, int npages) {
	int filePages = kv->mmap.total / BTREE_PAGE_SIZE;
	if (filePages >= npages) {
		return 0;
	}
	for (; filePages < npages;) {
		// the file size is increased exponentially,
		// so that we don't have to extend the file for every update.
		int inc = filePages / 8;
		if (inc < 1) {
			inc = 1;
		}
		filePages += inc;
	}
	int file_size = filePages * BTREE_PAGE_SIZE;

	HANDLE mmap = CreateFileMapping(kv->hfile, NULL, PAGE_READWRITE, 0, file_size, NULL);
	if (mmap == NULL)
	{
		return 1;
	}
	unsigned char* chunk = (unsigned char*)MapViewOfFile(mmap,
		FILE_MAP_ALL_ACCESS,
		0, 0, 0); //must consider the graduality of memory while determine dwFileOffset(High|Low)
	if (chunk == NULL) {
		CloseHandle(mmap);
		return 1;
	}

	kv->mmap.total = file_size;
	kv->mmap.oper_map = mmap;
	kv->mmap.chunk = chunk;

	return 0;
}

const char* DB_SIG = "BuildYourOwnDB05";
// the master page format.
// it contains the pointer to the root and other important bits.
// | sig | btree_root | page_used | free_list |
// | 16B |     8B     |     8B    |		8B	  |

/**
	@brief  Load muster page that contains info about database.
	@param  kv - Database structure to initialize.
	@retval    - Error code.
**/
int masterLoad(struct KV* kv) {
	if (kv->mmap.total == 0) {
		// empty file, the master page will be created on the first write.
		btnode initial_free_list_head = dump_create_np_s(BTREE_PAGE_SIZE);
		flnSetHeader(initial_free_list_head, 0, 0);
		flnSetTotal(initial_free_list_head, 0);
		kv->free.head = 0 + 1; //next to master page
		kv->free.use(kv->free.head, initial_free_list_head);
		kv->page.flushed = 2; // reserved for the master page and the head of free list
	}
	else
	{
		btptr root = mdlUtils.binary.little_endian.uint64(kv->mmap.chunk + 16);
		UINT64 used = mdlUtils.binary.little_endian.uint64(kv->mmap.chunk + 24);
		btptr free_list_head = mdlUtils.binary.little_endian.uint64(kv->mmap.chunk + 16 + 8 + 8);

		// verify the page
		if (memcmp(DB_SIG, kv->mmap.chunk, 16)) {
			return 1;//("Bad signature.")
		}
		boolean bad = !((1 <= used) && (used <= (kv->mmap.total / BTREE_PAGE_SIZE)));
		bad = bad || !((0 <= root) && (root < used));
		if (bad) {
			return 1;//("Bad master page.")
		}
		kv->tree.root = root;
		kv->page.flushed = used;
		kv->free.head = free_list_head;
	}
	return 0;
}

/**
	@brief  Update the master page. it must be atomic.
	Updating the page via mmap is not atomic. So use WriteFile function.
	@param  kv - Database structure to save master page.
	@retval    - Error code.
**/
int masterStore(struct KV* kv) {
	byte data[40];
	memcpy(data, DB_SIG, 16);
	mdlUtils.binary.little_endian.put_uint64(data + 16, kv->tree.root);
	mdlUtils.binary.little_endian.put_uint64(data + 24, kv->page.flushed);
	mdlUtils.binary.little_endian.put_uint64(data + 32, kv->free.head);
	DWORD bytes_actually_written;
	DWORD dwPtr = SetFilePointer(kv->hfile, 0, NULL, FILE_BEGIN);
	if (!WriteFile(kv->hfile, data, sizeof(data), &bytes_actually_written, NULL))
		return 1;
	else if (40 != bytes_actually_written)
		return 1;//("Error occured while writing master page")

	return 0;
}

/**
	@brief  Callback for BTree, allocate a new page.
	@param  db   - Database.
	@param  node - Description of page to insert.
	@retval      - Pointer to just inserted page.
**/
btptr pageNew(struct KV* db, btnode node) {
	// TODO: reuse deallocated pages
	assert(node.size <= BTREE_PAGE_SIZE);
	btptr ptr = 0;

	if (db->page.nfree < Total(&db->free)) {
		// reuse a deallocated page
		ptr = flGet(&db->free, db->page.nfree); //stack
		db->page.nfree++;
	}
	else {
		// append a new page
		ptr = db->page.flushed + db->page.nappend;
		db->page.nappend++;
	}
	struct mem_dump* packed_ptr = dump_pack(sizeof(btptr), &ptr, 1);
	insert(db->page.updates, packed_ptr, node);
	free(packed_ptr);
	return ptr;
}

/**
	@brief Callback for BTree, deallocate a page.
	@param db  - Database.
	@param ptr - Pointer to page that must be deleted.
**/
void pageDel(struct KV* db, btptr ptr) {
	struct mem_dump* packed_ptr = dump_pack(sizeof(btptr), &ptr, 1);
	struct mem_dump deallocated_page_value = __dump_pack(strlen("deallocated"), "deallocated");
	insert(db->page.updates, packed_ptr, deallocated_page_value);
	free(packed_ptr);
}

/**
	@brief  Callback for BTree, dereference a pointer.
	Dereference a pointer to flushed page.
	@param  kv  - Database.
	@param  ptr - Pointer to flushed page.
	@retval     - Page description.
**/
btnode pageGetMapped(struct KV* kv, btptr ptr/*number of page*/) {

	btptr start = 0;
	btptr end = start + kv->mmap.total / BTREE_PAGE_SIZE;
	if (ptr < end) {
		int offset = BTREE_PAGE_SIZE * (ptr - start);
		return __dump_pack((size_t)BTREE_PAGE_SIZE, kv->mmap.chunk + offset);
	}

	assert(0 && "bad ptr");
}

/**
	@brief  Callback for BTree and Free list, dereference a pointer.
	@param  kv  - Databse.
	@param  ptr - Pointer to page.
	@retval     - Page description.
**/
btnode pageGet(struct KV* kv, btptr ptr) {
	struct mem_dump result = { 0 };
	struct mem_dump* packed_ptr = dump_pack(sizeof(btptr), &ptr, 1);
	struct mem_dump page = get(kv->page.updates, packed_ptr);
	if (dump_cmp(&page, dump_dummy())) {
		assert(strcmp(page.dump, "deallocated"));
		result = page; // for new pages
	}
	else {
		result = pageGetMapped(kv, ptr); // for written pages
	}
	free(packed_ptr);
	return result;
}


int writePages(struct KV* db) {

	struct dynamic_array freed = da_create(sizeof(btptr));
	for (int i = 0; i < db->page.updates->size; i++)
	{
		if (!strcmp(db->page.updates->values[i].dump, "deallocated")) {
			btptr ptr = mdlUtils.binary.little_endian.uint64(db->page.updates->keys[i].dump);
			da_insert(&freed, &ptr, sizeof(btptr));
		}
	}
	Update(&db->free, db->page.nfree, &freed);

	// extend the file & mmap if needed
	int npages = db->page.flushed + db->page.nappend;
	int err;
	if (err = extend(db, npages)) {
		return err;
	}

	// copy data to the file
	for (int i = 0; i < db->page.updates->size; i++)
	{
		if (strcmp(db->page.updates->values[i].dump, "deallocated")) {
			btptr ptr = mdlUtils.binary.little_endian.uint64(db->page.updates->keys[i].dump);
			btnode node = pageGetMapped(db, ptr);
			memcpy_s(node.dump, node.size, db->page.updates->values[i].dump, db->page.updates->values[i].size);
		}
	}
	clear(db->page.updates);

	da_clear(&freed);
	return 0;
}

int syncPages(struct KV* db) {
	int err;
	// flush data to the disk. must be done before updating the master page.
	assert(FlushViewOfFile(db->mmap.chunk, db->mmap.total) && "Flush view failed.");
	db->page.flushed += db->page.nappend;
	db->page.nappend = 0;
	db->page.nfree = 0;
	// update & flush the master page
	if (err = masterStore(db)) {
		return err;
	}
	assert(FlushViewOfFile(db->mmap.chunk, db->mmap.total) && "Flush view failed.");
	return 0;
}

// persist the newly allocated pages after updates
int flushPages(struct KV* db) {
	int err = writePages(db);
	if (err) {
		return err;
	}
	return syncPages(db);
}

// cleanups same as Close
void kvstore_close(struct KV* db) {
	UnmapViewOfFile(db->mmap.chunk);
	CloseHandle(db->mmap.oper_map);
	CloseHandle(db->hfile);
}

int kvstore_open(struct KV* db, const char* name, const wchar_t* path,
	btnode(*btree_get_callback)(btptr ptr),
	btptr(*btree_new_callback)(btnode node),
	void (*btree_del_callback)(btptr ptr),
	btnode(*free_list_get_callback)(btptr ptr),
	btptr(*free_list_append_callback)(btnode node),
	void (*free_list_use_callback)(btptr ptr, btnode node)) {

	if (wcslen(path) > MAX_PATH)
		return 1;

	db->path = (wchar_t*)malloc((wcslen(path) + 1) * sizeof(wchar_t));
	if (db->path == NULL)
		return 1;
	else wcscpy_s(db->path, wcslen(path) + 1, path);

	db->tree.name = (char*)malloc(strlen(name) + 1);
	if (db->tree.name == NULL)
		return 1;
	else strcpy_s(db->tree.name, strlen(name) + 1, name);

	// btree callbacks
	db->tree.get = btree_get_callback;
	db->tree.allocate = btree_new_callback;
	db->tree.del = btree_del_callback;

	int status;

	if ((db->hfile = CreateFileW(db->path, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_WRITE,
		NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL,
		NULL)) == (HANDLE)INVALID_HANDLE_VALUE) {
		return 1;
	}

	if ((status = mmap_init(db->hfile, &db->mmap.oper_map, &db->mmap.total, &db->mmap.chunk)) != 0) {
		if (!((status == 3) && (db->mmap.total == 0))) {
			kvstore_close(db);
			return 1;
		}
	}

	//page member initialization
	db->page.nfree = 0;
	db->page.nappend = 0;
	db->page.updates = map_create();

	db->free.get = free_list_get_callback;
	db->free.append = free_list_append_callback;
	db->free.use = free_list_use_callback;

	// read the master page
	if (status = masterLoad(db)) {
		kvstore_close(db);
		return 1;
	}
	return 0;
}

value* Get(struct KV* db, key* key) {
	return TreeGet(&db->tree, key);
}

int Set(struct KV* db, key* key, value* val) {
	Insert(&db->tree, key, val);
	return flushPages(db);
}

int Del(struct KV* db, key* key) {
	boolean deleted = Delete(&db->tree, key);
	return flushPages(db);
}

// callback for FreeList, allocate a new page.
btptr pageAppend(struct KV* db, btnode node) {
	assert(node.size <= BTREE_PAGE_SIZE);
	btptr ptr = db->page.flushed + db->page.nappend;
	db->page.nappend++;
	pageUse(db, ptr, node);
	return ptr;
}

// callback for FreeList, reuse a page.
void pageUse(struct KV* db, btptr ptr, btnode node) {
	struct mem_dump* packed_ptr = dump_pack(sizeof(btptr), &ptr, 1);
	insert(db->page.updates, packed_ptr, node);
	free(packed_ptr);
}