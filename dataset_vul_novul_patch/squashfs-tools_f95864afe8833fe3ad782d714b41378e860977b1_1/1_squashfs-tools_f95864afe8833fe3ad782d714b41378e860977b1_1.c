 int read_filesystem_tables_4()
 {
	long long directory_table_end, table_start;
 
	if(read_xattrs_from_disk(fd, &sBlk.s, no_xattrs, &table_start) == 0)
		return FALSE;
 
	if(read_uids_guids(&table_start) == FALSE)
		return FALSE;
 
	if(parse_exports_table(&table_start) == FALSE)
		return FALSE;
 
	if(read_fragment_table(&directory_table_end) == FALSE)
		return FALSE;
 
	if(read_inode_table(sBlk.s.inode_table_start,
				sBlk.s.directory_table_start) == FALSE)
		return FALSE;
 
 	if(read_directory_table(sBlk.s.directory_table_start,
				directory_table_end) == FALSE)
		return FALSE;
 
 	if(no_xattrs)
 		sBlk.s.xattr_id_table_start = SQUASHFS_INVALID_BLK;
 
 	return TRUE;
 }
