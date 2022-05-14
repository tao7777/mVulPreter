ext4_ext_handle_uninitialized_extents(handle_t *handle, struct inode *inode,
			ext4_lblk_t iblock, unsigned int max_blocks,
			struct ext4_ext_path *path, int flags,
			unsigned int allocated, struct buffer_head *bh_result,
			ext4_fsblk_t newblock)
{
	int ret = 0;
	int err = 0;
	ext4_io_end_t *io = EXT4_I(inode)->cur_aio_dio;

	ext_debug("ext4_ext_handle_uninitialized_extents: inode %lu, logical"
		  "block %llu, max_blocks %u, flags %d, allocated %u",
		  inode->i_ino, (unsigned long long)iblock, max_blocks,
		  flags, allocated);
 	ext4_ext_show_leaf(inode, path);
 
 	/* get_block() before submit the IO, split the extent */
	if (flags == EXT4_GET_BLOCKS_PRE_IO) {
 		ret = ext4_split_unwritten_extents(handle,
 						inode, path, iblock,
 						max_blocks, flags);
		/*
		 * Flag the inode(non aio case) or end_io struct (aio case)
		 * that this IO needs to convertion to written when IO is
		 * completed
		 */
		if (io)
 			io->flag = EXT4_IO_UNWRITTEN;
 		else
 			ext4_set_inode_state(inode, EXT4_STATE_DIO_UNWRITTEN);
 		goto out;
 	}
 	/* IO end_io complete, convert the filled extent to written */
	if (flags == EXT4_GET_BLOCKS_CONVERT) {
 		ret = ext4_convert_unwritten_extents_endio(handle, inode,
 							path);
 		if (ret >= 0)
			ext4_update_inode_fsync_trans(handle, inode, 1);
		goto out2;
	}
	/* buffered IO case */
	/*
	 * repeat fallocate creation request
	 * we already have an unwritten extent
	 */
	if (flags & EXT4_GET_BLOCKS_UNINIT_EXT)
		goto map_out;

	/* buffered READ or buffered write_begin() lookup */
	if ((flags & EXT4_GET_BLOCKS_CREATE) == 0) {
		/*
		 * We have blocks reserved already.  We
		 * return allocated blocks so that delalloc
		 * won't do block reservation for us.  But
		 * the buffer head will be unmapped so that
		 * a read from the block returns 0s.
		 */
		set_buffer_unwritten(bh_result);
		goto out1;
	}

	/* buffered write, writepage time, convert*/
	ret = ext4_ext_convert_to_initialized(handle, inode,
						path, iblock,
						max_blocks);
	if (ret >= 0)
		ext4_update_inode_fsync_trans(handle, inode, 1);
out:
	if (ret <= 0) {
		err = ret;
		goto out2;
	} else
		allocated = ret;
	set_buffer_new(bh_result);
	/*
	 * if we allocated more blocks than requested
	 * we need to make sure we unmap the extra block
	 * allocated. The actual needed block will get
	 * unmapped later when we find the buffer_head marked
	 * new.
	 */
	if (allocated > max_blocks) {
		unmap_underlying_metadata_blocks(inode->i_sb->s_bdev,
					newblock + max_blocks,
					allocated - max_blocks);
		allocated = max_blocks;
	}

	/*
	 * If we have done fallocate with the offset that is already
	 * delayed allocated, we would have block reservation
	 * and quota reservation done in the delayed write path.
	 * But fallocate would have already updated quota and block
	 * count for this offset. So cancel these reservation
	 */
	if (flags & EXT4_GET_BLOCKS_DELALLOC_RESERVE)
		ext4_da_update_reserve_space(inode, allocated, 0);

map_out:
	set_buffer_mapped(bh_result);
out1:
	if (allocated > max_blocks)
		allocated = max_blocks;
	ext4_ext_show_leaf(inode, path);
	bh_result->b_bdev = inode->i_sb->s_bdev;
	bh_result->b_blocknr = newblock;
out2:
	if (path) {
		ext4_ext_drop_refs(path);
		kfree(path);
	}
	return err ? err : allocated;
}
