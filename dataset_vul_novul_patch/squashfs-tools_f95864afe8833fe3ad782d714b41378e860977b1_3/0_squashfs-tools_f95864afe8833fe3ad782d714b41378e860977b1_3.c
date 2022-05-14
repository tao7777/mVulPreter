static int read_uids_guids(long long *table_start)
static int read_id_table(long long *table_start)
 {
	/*
	 * Note on overflow limits:
	 * Size of SBlk.s.no_ids is 2^16 (unsigned short)
	 * Max size of bytes is 2^16*4 or 256K
	 * Max indexes is (2^16*4)/8K or 32
	 * Max length is ((2^16*4)/8K)*8 or 256
	 */
 	int res, i;
 	int bytes = SQUASHFS_ID_BYTES(sBlk.s.no_ids);
 	int indexes = SQUASHFS_ID_BLOCKS(sBlk.s.no_ids);
	int length = SQUASHFS_ID_BLOCK_BYTES(sBlk.s.no_ids);
	long long *id_index_table;

	/*
	 * The size of the index table (length bytes) should match the
	 * table start and end points
	 */
	if(length != (*table_start - sBlk.s.id_table_start)) {
		ERROR("read_id_table: Bad id count in super block\n");
		return FALSE;
	}
 
	TRACE("read_id_table: no_ids %d\n", sBlk.s.no_ids);
 
	id_index_table = alloc_index_table(indexes);
 	id_table = malloc(bytes);
 	if(id_table == NULL) {
		ERROR("read_id_table: failed to allocate id table\n");
 		return FALSE;
 	}
 
	res = read_fs_bytes(fd, sBlk.s.id_table_start, length, id_index_table);
 	if(res == FALSE) {
		ERROR("read_id_table: failed to read id index table\n");
 		return FALSE;
 	}
 	SQUASHFS_INSWAP_ID_BLOCKS(id_index_table, indexes);

	/*
	 * id_index_table[0] stores the start of the compressed id blocks.
	 * This by definition is also the end of the previous filesystem
	 * table - this may be the exports table if it is present, or the
	 * fragments table if it isn't.
	 */
	*table_start = id_index_table[0];

	for(i = 0; i < indexes; i++) {
		int expected = (i + 1) != indexes ? SQUASHFS_METADATA_SIZE :
					bytes & (SQUASHFS_METADATA_SIZE - 1);
 		res = read_block(fd, id_index_table[i], NULL, expected,
 			((char *) id_table) + i * SQUASHFS_METADATA_SIZE);
 		if(res == FALSE) {
			ERROR("read_id_table: failed to read id table block"
 				"\n");
 			return FALSE;
 		}
	}

	SQUASHFS_INSWAP_INTS(id_table, sBlk.s.no_ids);

	return TRUE;
}
