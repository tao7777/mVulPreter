static int mpage_da_map_blocks(struct mpage_da_data *mpd)
{
	int err, blks, get_blocks_flags;
	struct buffer_head new;
	sector_t next = mpd->b_blocknr;
	unsigned max_blocks = mpd->b_size >> mpd->inode->i_blkbits;
	loff_t disksize = EXT4_I(mpd->inode)->i_disksize;
	handle_t *handle = NULL;

	/*
	 * We consider only non-mapped and non-allocated blocks
	 */
	if ((mpd->b_state  & (1 << BH_Mapped)) &&
		!(mpd->b_state & (1 << BH_Delay)) &&
		!(mpd->b_state & (1 << BH_Unwritten)))
		return 0;

	/*
	 * If we didn't accumulate anything to write simply return
	 */
	if (!mpd->b_size)
		return 0;

	handle = ext4_journal_current_handle();
	BUG_ON(!handle);

	/*
	 * Call ext4_get_blocks() to allocate any delayed allocation
	 * blocks, or to convert an uninitialized extent to be
	 * initialized (in the case where we have written into
	 * one or more preallocated blocks).
	 *
	 * We pass in the magic EXT4_GET_BLOCKS_DELALLOC_RESERVE to
	 * indicate that we are on the delayed allocation path.  This
	 * affects functions in many different parts of the allocation
	 * call path.  This flag exists primarily because we don't
	 * want to change *many* call functions, so ext4_get_blocks()
	 * will set the magic i_delalloc_reserved_flag once the
	 * inode's allocation semaphore is taken.
	 *
	 * If the blocks in questions were delalloc blocks, set
	 * EXT4_GET_BLOCKS_DELALLOC_RESERVE so the delalloc accounting
	 * variables are updated after the blocks have been allocated.
 	 */
 	new.b_state = 0;
 	get_blocks_flags = EXT4_GET_BLOCKS_CREATE;
	if (ext4_should_dioread_nolock(mpd->inode))
		get_blocks_flags |= EXT4_GET_BLOCKS_IO_CREATE_EXT;
 	if (mpd->b_state & (1 << BH_Delay))
 		get_blocks_flags |= EXT4_GET_BLOCKS_DELALLOC_RESERVE;
 
	blks = ext4_get_blocks(handle, mpd->inode, next, max_blocks,
			       &new, get_blocks_flags);
	if (blks < 0) {
		err = blks;
		/*
		 * If get block returns with error we simply
		 * return. Later writepage will redirty the page and
		 * writepages will find the dirty page again
		 */
		if (err == -EAGAIN)
			return 0;

		if (err == -ENOSPC &&
		    ext4_count_free_blocks(mpd->inode->i_sb)) {
			mpd->retval = err;
			return 0;
		}

		/*
		 * get block failure will cause us to loop in
		 * writepages, because a_ops->writepage won't be able
		 * to make progress. The page will be redirtied by
		 * writepage and writepages will again try to write
		 * the same.
		 */
		ext4_msg(mpd->inode->i_sb, KERN_CRIT,
			 "delayed block allocation failed for inode %lu at "
			 "logical offset %llu with max blocks %zd with "
			 "error %d\n", mpd->inode->i_ino,
			 (unsigned long long) next,
			 mpd->b_size >> mpd->inode->i_blkbits, err);
		printk(KERN_CRIT "This should not happen!!  "
		       "Data will be lost\n");
		if (err == -ENOSPC) {
			ext4_print_free_blocks(mpd->inode);
		}
		/* invalidate all the pages */
		ext4_da_block_invalidatepages(mpd, next,
				mpd->b_size >> mpd->inode->i_blkbits);
		return err;
	}
	BUG_ON(blks == 0);

	new.b_size = (blks << mpd->inode->i_blkbits);

	if (buffer_new(&new))
		__unmap_underlying_blocks(mpd->inode, &new);

	/*
	 * If blocks are delayed marked, we need to
	 * put actual blocknr and drop delayed bit
	 */
	if ((mpd->b_state & (1 << BH_Delay)) ||
	    (mpd->b_state & (1 << BH_Unwritten)))
		mpage_put_bnr_to_bhs(mpd, next, &new);

	if (ext4_should_order_data(mpd->inode)) {
		err = ext4_jbd2_file_inode(handle, mpd->inode);
		if (err)
			return err;
	}

	/*
	 * Update on-disk size along with block allocation.
	 */
	disksize = ((loff_t) next + blks) << mpd->inode->i_blkbits;
	if (disksize > i_size_read(mpd->inode))
		disksize = i_size_read(mpd->inode);
	if (disksize > EXT4_I(mpd->inode)->i_disksize) {
		ext4_update_i_disksize(mpd->inode, disksize);
		return ext4_mark_inode_dirty(handle, mpd->inode);
	}

	return 0;
}
