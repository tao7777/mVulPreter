 static int parse_exports_table(long long *table_start)
 {
	/*
	 * Note on overflow limits:
	 * Size of SBlk.s.inodes is 2^32 (unsigned int)
	 * Max indexes is (2^32*8)/8K or 2^22
	 * Max length is ((2^32*8)/8K)*8 or 2^25
	 */
 	int res;
	int indexes = SQUASHFS_LOOKUP_BLOCKS((long long) sBlk.s.inodes);
	int length = SQUASHFS_LOOKUP_BLOCK_BYTES((long long) sBlk.s.inodes);
	long long *export_index_table;

	/*
	 * The size of the index table (length bytes) should match the
	 * table start and end points
	 */
	if(length != (*table_start - sBlk.s.lookup_table_start)) {
		ERROR("parse_exports_table: Bad inode count in super block\n");
		return FALSE;
	}
 
	export_index_table = alloc_index_table(indexes);

	res = read_fs_bytes(fd, sBlk.s.lookup_table_start, length,
							export_index_table);
 	if(res == FALSE) {
 		ERROR("parse_exports_table: failed to read export index table\n");
 		return FALSE;
	}
	SQUASHFS_INSWAP_LOOKUP_BLOCKS(export_index_table, indexes);

	/*
	 * export_index_table[0] stores the start of the compressed export blocks.
	 * This by definition is also the end of the previous filesystem
	 * table - the fragment table.
	 */
	*table_start = export_index_table[0];

	return TRUE;
}
