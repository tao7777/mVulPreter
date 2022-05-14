 int read_filesystem_tables_4()
 {
	long long table_start;
 
	/* Read xattrs */
	if(sBlk.s.xattr_id_table_start != SQUASHFS_INVALID_BLK) {
		/* sanity check super block contents */
		if(sBlk.s.xattr_id_table_start >= sBlk.s.bytes_used) {
			ERROR("read_filesystem_tables: xattr id table start too large in super block\n");
			goto corrupted;
		}
 
		if(read_xattrs_from_disk(fd, &sBlk.s, no_xattrs, &table_start) == 0)
			goto corrupted;
	} else
		table_start = sBlk.s.bytes_used;
 
	/* Read id lookup table */
 
	/* Sanity check super block contents */
	if(sBlk.s.id_table_start >= table_start) {
		ERROR("read_filesystem_tables: id table start too large in super block\n");
		goto corrupted;
	}
 
	/* there should always be at least one id */
	if(sBlk.s.no_ids == 0) {
		ERROR("read_filesystem_tables: Bad id count in super block\n");
		goto corrupted;
	}

	/*
	 * the number of ids can never be more than double the number of inodes
	 * (the maximum is a unique uid and gid for each inode).
	 */
	if(sBlk.s.no_ids > (sBlk.s.inodes * 2L)) {
		ERROR("read_filesystem_tables: Bad id count in super block\n");
		goto corrupted;
	}

	if(read_id_table(&table_start) == FALSE)
		goto corrupted;

	/* Read exports table */
	if(sBlk.s.lookup_table_start != SQUASHFS_INVALID_BLK) {

		/* sanity check super block contents */
		if(sBlk.s.lookup_table_start >= table_start) {
			ERROR("read_filesystem_tables: lookup table start too large in super block\n");
			goto corrupted;
		}

		if(parse_exports_table(&table_start) == FALSE)
			goto corrupted;
	}

	/* Read fragment table */
	if(sBlk.s.fragments != 0) {

		/* Sanity check super block contents */
		if(sBlk.s.fragment_table_start >= table_start) {
			ERROR("read_filesystem_tables: fragment table start too large in super block\n");
			goto corrupted;
		}

		/* The number of fragments should not exceed the number of inodes */
		if(sBlk.s.fragments > sBlk.s.inodes) {
			ERROR("read_filesystem_tables: Bad fragment count in super block\n");
			goto corrupted;
		}

		if(read_fragment_table(&table_start) == FALSE)
			goto corrupted;
	} else {
		/*
		 * Sanity check super block contents - with 0 fragments,
		 * the fragment table should be empty
		 */
		if(sBlk.s.fragment_table_start != table_start) {
			ERROR("read_filesystem_tables: fragment table start invalid in super block\n");
			goto corrupted;
		}
	}

	/* Read directory table */

	/* Sanity check super block contents */
	if(sBlk.s.directory_table_start >= table_start) {
		ERROR("read_filesystem_tables: directory table start too large in super block\n");
		goto corrupted;
	}
 
 	if(read_directory_table(sBlk.s.directory_table_start,
				table_start) == FALSE)
		goto corrupted;

	/* Read inode table */

	/* Sanity check super block contents */
	if(sBlk.s.inode_table_start >= sBlk.s.directory_table_start) {
		ERROR("read_filesystem_tables: inode table start too large in super block\n");
		goto corrupted;
	}

	if(read_inode_table(sBlk.s.inode_table_start,
				sBlk.s.directory_table_start) == FALSE)
		goto corrupted;
 
 	if(no_xattrs)
 		sBlk.s.xattr_id_table_start = SQUASHFS_INVALID_BLK;
 
 	return TRUE;

corrupted:
	ERROR("File system corruption detected\n");
	return FALSE;
 }
