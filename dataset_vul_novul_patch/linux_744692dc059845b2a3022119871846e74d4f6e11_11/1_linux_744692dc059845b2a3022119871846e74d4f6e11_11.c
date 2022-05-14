 static int ext4_write_begin(struct file *file, struct address_space *mapping,
 			    loff_t pos, unsigned len, unsigned flags,
 			    struct page **pagep, void **fsdata)
{
	struct inode *inode = mapping->host;
	int ret, needed_blocks;
	handle_t *handle;
	int retries = 0;
	struct page *page;
	pgoff_t index;
	unsigned from, to;

	trace_ext4_write_begin(inode, pos, len, flags);
	/*
	 * Reserve one block more for addition to orphan list in case
	 * we allocate blocks but write fails for some reason
	 */
	needed_blocks = ext4_writepage_trans_blocks(inode) + 1;
	index = pos >> PAGE_CACHE_SHIFT;
	from = pos & (PAGE_CACHE_SIZE - 1);
	to = from + len;

retry:
	handle = ext4_journal_start(inode, needed_blocks);
	if (IS_ERR(handle)) {
		ret = PTR_ERR(handle);
		goto out;
	}

	/* We cannot recurse into the filesystem as the transaction is already
	 * started */
	flags |= AOP_FLAG_NOFS;

	page = grab_cache_page_write_begin(mapping, index, flags);
	if (!page) {
		ext4_journal_stop(handle);
		ret = -ENOMEM;
		goto out;
 	}
 	*pagep = page;
 
	ret = block_write_begin(file, mapping, pos, len, flags, pagep, fsdata,
				ext4_get_block);
 
 	if (!ret && ext4_should_journal_data(inode)) {
 		ret = walk_page_buffers(handle, page_buffers(page),
				from, to, NULL, do_journal_get_write_access);
	}

	if (ret) {
		unlock_page(page);
		page_cache_release(page);
		/*
		 * block_write_begin may have instantiated a few blocks
		 * outside i_size.  Trim these off again. Don't need
		 * i_size_read because we hold i_mutex.
		 *
		 * Add inode to orphan list in case we crash before
		 * truncate finishes
		 */
		if (pos + len > inode->i_size && ext4_can_truncate(inode))
			ext4_orphan_add(handle, inode);

		ext4_journal_stop(handle);
		if (pos + len > inode->i_size) {
			ext4_truncate_failed_write(inode);
			/*
			 * If truncate failed early the inode might
			 * still be on the orphan list; we need to
			 * make sure the inode is removed from the
			 * orphan list in that case.
			 */
			if (inode->i_nlink)
				ext4_orphan_del(NULL, inode);
		}
	}

	if (ret == -ENOSPC && ext4_should_retry_alloc(inode->i_sb, &retries))
		goto retry;
out:
	return ret;
}
