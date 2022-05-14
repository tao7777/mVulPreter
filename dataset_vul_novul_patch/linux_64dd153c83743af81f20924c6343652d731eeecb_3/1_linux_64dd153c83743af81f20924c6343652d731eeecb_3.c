static int needs_empty_write(sector_t block, struct inode *inode)
{
	int error;
	struct buffer_head bh_map = { .b_state = 0, .b_blocknr = 0 };
	bh_map.b_size = 1 << inode->i_blkbits;
	error = gfs2_block_map(inode, block, &bh_map, 0);
	if (unlikely(error))
		return error;
	return !buffer_mapped(&bh_map);
}
