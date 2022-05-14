static int read_fragment_table(long long *directory_table_end)
long long *alloc_index_table(int indexes)
 {
	static long long *alloc_table = NULL;
	static int alloc_size = 0;
	int length = indexes * sizeof(long long);

	if(alloc_size < length) {
		long long *table = realloc(alloc_table, length);

		if(table == NULL)
			EXIT_UNSQUASH("alloc_index_table: failed to allocate "
				"index table\n");

		alloc_table = table;
		alloc_size = length;
	}

	return alloc_table;
}


static int read_fragment_table(long long *table_start)
{
	/*
	 * Note on overflow limits:
	 * Size of SBlk.s.fragments is 2^32 (unsigned int)
	 * Max size of bytes is 2^32*16 or 2^36
	 * Max indexes is (2^32*16)/8K or 2^23
	 * Max length is ((2^32*16)/8K)*8 or 2^26 or 64M
	 */
 	int res, i;
	long long bytes = SQUASHFS_FRAGMENT_BYTES((long long) sBlk.s.fragments);
	int indexes = SQUASHFS_FRAGMENT_INDEXES((long long) sBlk.s.fragments);
	int length = SQUASHFS_FRAGMENT_INDEX_BYTES((long long) sBlk.s.fragments);
	long long *fragment_table_index;

	/*
	 * The size of the index table (length bytes) should match the
	 * table start and end points
	 */
	if(length != (*table_start - sBlk.s.fragment_table_start)) {
		ERROR("read_fragment_table: Bad fragment count in super block\n");
		return FALSE;
	}
 
 	TRACE("read_fragment_table: %d fragments, reading %d fragment indexes "
 		"from 0x%llx\n", sBlk.s.fragments, indexes,
 		sBlk.s.fragment_table_start);
 
	fragment_table_index = alloc_index_table(indexes);
 	fragment_table = malloc(bytes);
 	if(fragment_table == NULL)
 		EXIT_UNSQUASH("read_fragment_table: failed to allocate "
 			"fragment table\n");
 
	res = read_fs_bytes(fd, sBlk.s.fragment_table_start, length,
							fragment_table_index);
 	if(res == FALSE) {
 		ERROR("read_fragment_table: failed to read fragment table "
 			"index\n");
		return FALSE;
	}
	SQUASHFS_INSWAP_FRAGMENT_INDEXES(fragment_table_index, indexes);

	for(i = 0; i < indexes; i++) {
		int expected = (i + 1) != indexes ? SQUASHFS_METADATA_SIZE :
					bytes & (SQUASHFS_METADATA_SIZE - 1);
		int length = read_block(fd, fragment_table_index[i], NULL,
			expected, ((char *) fragment_table) + (i *
			SQUASHFS_METADATA_SIZE));
		TRACE("Read fragment table block %d, from 0x%llx, length %d\n",
			i, fragment_table_index[i], length);
		if(length == FALSE) {
			ERROR("read_fragment_table: failed to read fragment "
				"table index\n");
			return FALSE;
		}
	}

 	for(i = 0; i < sBlk.s.fragments; i++) 
 		SQUASHFS_INSWAP_FRAGMENT_ENTRY(&fragment_table[i]);
 
	*table_start = fragment_table_index[0];
 	return TRUE;
 }
