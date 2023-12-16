#pragma once

// extend the mmap by adding new mappings.
/*int extendMmap(struct KV* kv, int npages) {

	if (kv->mmap.total >= npages * BTREE_PAGE_SIZE) {

		SYSTEM_INFO sys_info = {0};
		GetSystemInfo(&sys_info);
		DWORD granularity = sys_info.dwAllocationGranularity;

		int accessed_address = npages * BTREE_PAGE_SIZE;
		int granularity_page = accessed_address / granularity;
		int chunks_count = count_mem_dump_nodes(kv->mmap.chunks);

		//before target chunks
		for (int i = 0; i < granularity_page; i++)
		{
			struct mem_dump node = get_mem_dump_node(kv->mmap.chunks, i);
			if (node.size != granularity) {
				UnmapViewOfFile(node.dump);
				unsigned char* chunk = (unsigned char*)MapViewOfFile(kv->mmap.oper_map, FILE_MAP_ALL_ACCESS,
					0, granularity * i, granularity);
			}
		}

		unsigned char* target_chunk = (unsigned char*)MapViewOfFile(kv->mmap.oper_map, FILE_MAP_ALL_ACCESS,
			0, granularity * granularity_page, kv->mmap.file - granularity * granularity_page);
		add_mem_dump_node(kv->mmap.chunks, __dump_pack(kv->mmap.file - granularity * granularity_page, target_chunk));

		return 0; //already mmap larger than required
	}

	UnmapViewOfFile(kv->mmap.chunk);
	CloseHandle(kv->mmap.oper_map);

	if (kv->mmap.total == 0)
	{
		kv->mmap.total = kv->mmap.total * 2;
	}
	else kv->mmap.total += kv->mmap.total;

	// double the address space
	HANDLE mmap = CreateFileMapping(kv->hfile, NULL, PAGE_READWRITE, kv->mmap.total >> sizeof(DWORD) * 8, kv->mmap.total, NULL);
	if (mmap == NULL)
	{
		return 2;
	}
	printf("file size: %d\n", GetFileSize(kv->hfile, NULL));
	dwNumberOfBytesToMap
		[in] Specifies the number of bytes of the file to map.If dwNumberOfBytesToMap is zero, the entire file is mapped.
	unsigned char* chunk = (unsigned char*)MapViewOfFile(mmap,
		FILE_MAP_ALL_ACCESS,
		0, 0, 0); //must consider the graduality of memory while determine dwFileOffset(High|Low)
	if (chunk == NULL) {
		return 3;
	}

	kv->mmap.oper_map = mmap;
	kv->mmap.chunk = chunk;

	return 0;
}

// extend the file to at least `npages`.
int extendFile(struct KV* db, int npages) {
	int filePages = db->mmap.total / BTREE_PAGE_SIZE;
	if (filePages >= npages) {
		return 0;
	}
	for (;filePages < npages;) {
		// the file size is increased exponentially,
		// so that we don't have to extend the file for every update.
		int inc = filePages / 8;
		if (inc < 1) {
			inc = 1;
		}
		filePages += inc;
	}
	int file_size = filePages * BTREE_PAGE_SIZE;
	//https://www.perplexity.ai/search/c5d3010c-97d6-4dc0-ac19-68a5b6f5edcf?s=u
	if (!SetFilePointer(db->hfile, file_size, NULL, FILE_BEGIN))
	{
		return 1;
	}
	if (!SetEndOfFile(db->hfile))
	{
		DWORD err = GetLastError();
		return 2;
	}
	SetFilePointer(db->hfile, 0, NULL, FILE_BEGIN);
	db->mmap.total = file_size;
	return 0;
}*/