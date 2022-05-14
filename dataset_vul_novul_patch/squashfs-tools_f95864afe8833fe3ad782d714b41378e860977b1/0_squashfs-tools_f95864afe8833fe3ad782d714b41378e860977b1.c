 int read_xattrs_from_disk(int fd, struct squashfs_super_block *sBlk, int flag, long long *table_start)
 {
	/*
	 * Note on overflow limits:
	 * Size of ids (id_table.xattr_ids) is 2^32 (unsigned int)
	 * Max size of bytes is 2^32*16 or 2^36
	 * Max indexes is (2^32*16)/8K or 2^23
	 * Max index_bytes is ((2^32*16)/8K)*8 or 2^26 or 64M
	 */
	int res, i, indexes, index_bytes;
	unsigned int ids;
	long long bytes;
 	long long *index, start, end;
 	struct squashfs_xattr_table id_table;
 
	TRACE("read_xattrs_from_disk\n");

	if(sBlk->xattr_id_table_start == SQUASHFS_INVALID_BLK)
		return SQUASHFS_INVALID_BLK;

	/*
	 * Read xattr id table, containing start of xattr metadata and the
	 * number of xattrs in the file system
	 */
	res = read_fs_bytes(fd, sBlk->xattr_id_table_start, sizeof(id_table),
		&id_table);
	if(res == 0)
		return 0;
 
 	SQUASHFS_INSWAP_XATTR_TABLE(&id_table);
 
	/*
	 * Compute index table values
	 */
	ids = id_table.xattr_ids;
	xattr_table_start = id_table.xattr_table_start;
	index_bytes = SQUASHFS_XATTR_BLOCK_BYTES((long long) ids);
	indexes = SQUASHFS_XATTR_BLOCKS((long long) ids);

	/*
	 * The size of the index table (index_bytes) should match the
	 * table start and end points
	 */
	if(index_bytes != (sBlk->bytes_used - (sBlk->xattr_id_table_start + sizeof(id_table)))) {
		ERROR("read_xattrs_from_disk: Bad xattr_ids count in super block\n");
		return 0;
	}

	/*
	 * id_table.xattr_table_start stores the start of the compressed xattr
	 * metadata blocks.  This by definition is also the end of the previous
	 * filesystem table - the id lookup table.
	 */
	if(table_start != NULL)
 		*table_start = id_table.xattr_table_start;

	/*
	 * If flag is set then return once we've read the above
	 * table_start.  That value is necessary for sanity checking,
	 * but we don't actually want to extract the xattrs, and so
	 * stop here.
	 */
	if(flag)
 		return id_table.xattr_ids;
 
 	/*
 	 * Allocate and read the index to the xattr id table metadata
 	 * blocks
 	 */
 	index = malloc(index_bytes);
 	if(index == NULL)
 		MEM_ERROR();

	res = read_fs_bytes(fd, sBlk->xattr_id_table_start + sizeof(id_table),
		index_bytes, index);
	if(res ==0)
		goto failed1;

	SQUASHFS_INSWAP_LONG_LONGS(index, indexes);

	/*
 	 * Allocate enough space for the uncompressed xattr id table, and
 	 * read and decompress it
 	 */
	bytes = SQUASHFS_XATTR_BYTES((long long) ids);
 	xattr_ids = malloc(bytes);
 	if(xattr_ids == NULL)
 		MEM_ERROR();

	for(i = 0; i < indexes; i++) {
		int expected = (i + 1) != indexes ? SQUASHFS_METADATA_SIZE :
 					bytes & (SQUASHFS_METADATA_SIZE - 1);
 		int length = read_block(fd, index[i], NULL, expected,
 			((unsigned char *) xattr_ids) +
			((long long) i * SQUASHFS_METADATA_SIZE));
 		TRACE("Read xattr id table block %d, from 0x%llx, length "
 			"%d\n", i, index[i], length);
 		if(length == 0) {
			ERROR("Failed to read xattr id table block %d, "
				"from 0x%llx, length %d\n", i, index[i],
				length);
			goto failed2;
		}
	}

	/*
	 * Read and decompress the xattr metadata
	 *
	 * Note the first xattr id table metadata block is immediately after
	 * the last xattr metadata block, so we can use index[0] to work out
	 * the end of the xattr metadata
	 */
	start = xattr_table_start;
	end = index[0];
	for(i = 0; start < end; i++) {
		int length;
		xattrs = realloc(xattrs, (i + 1) * SQUASHFS_METADATA_SIZE);
		if(xattrs == NULL)
			MEM_ERROR();

		/* store mapping from location of compressed block in fs ->
		 * location of uncompressed block in memory */
		save_xattr_block(start, i * SQUASHFS_METADATA_SIZE);

		length = read_block(fd, start, &start, 0,
			((unsigned char *) xattrs) +
			(i * SQUASHFS_METADATA_SIZE));
		TRACE("Read xattr block %d, length %d\n", i, length);
		if(length == 0) {
			ERROR("Failed to read xattr block %d\n", i);
			goto failed3;
		}

		/*
		 * If this is not the last metadata block in the xattr metadata
		 * then it should be SQUASHFS_METADATA_SIZE in size.
		 * Note, we can't use expected in read_block() above for this
		 * because we don't know if this is the last block until
		 * after reading.
		 */
		if(start != end && length != SQUASHFS_METADATA_SIZE) {
			ERROR("Xattr block %d should be %d bytes in length, "
				"it is %d bytes\n", i, SQUASHFS_METADATA_SIZE,
				length);
			goto failed3;
		}
	}

	/* swap if necessary the xattr id entries */
	for(i = 0; i < ids; i++)
		SQUASHFS_INSWAP_XATTR_ID(&xattr_ids[i]);

	free(index);

	return ids;

failed3:
	free(xattrs);
failed2:
	free(xattr_ids);
failed1:
	free(index);

	return 0;
}
