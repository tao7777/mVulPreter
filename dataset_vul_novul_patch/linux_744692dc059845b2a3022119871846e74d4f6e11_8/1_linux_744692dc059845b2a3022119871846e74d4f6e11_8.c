 static int ext4_get_block_write(struct inode *inode, sector_t iblock,
 		   struct buffer_head *bh_result, int create)
 {
	handle_t *handle = NULL;
 	int ret = 0;
 	unsigned max_blocks = bh_result->b_size >> inode->i_blkbits;
 	int dio_credits;
 
 	ext4_debug("ext4_get_block_write: inode %lu, create flag %d\n",
 		   inode->i_ino, create);
	/*
	 * ext4_get_block in prepare for a DIO write or buffer write.
	 * We allocate an uinitialized extent if blocks haven't been allocated.
	 * The extent will be converted to initialized after IO complete.
 	 */
 	create = EXT4_GET_BLOCKS_IO_CREATE_EXT;
 
	if (max_blocks > DIO_MAX_BLOCKS)
		max_blocks = DIO_MAX_BLOCKS;
	dio_credits = ext4_chunk_trans_blocks(inode, max_blocks);
	handle = ext4_journal_start(inode, dio_credits);
	if (IS_ERR(handle)) {
		ret = PTR_ERR(handle);
		goto out;
 	}
 	ret = ext4_get_blocks(handle, inode, iblock, max_blocks, bh_result,
 			      create);
 	if (ret > 0) {
 		bh_result->b_size = (ret << inode->i_blkbits);
 		ret = 0;
 	}
	ext4_journal_stop(handle);
 out:
 	return ret;
 }
