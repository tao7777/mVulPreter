static int empty_write_end(struct page *page, unsigned from,
			   unsigned to, int mode)
{
	struct inode *inode = page->mapping->host;
	struct gfs2_inode *ip = GFS2_I(inode);
	struct buffer_head *bh;
	unsigned offset, blksize = 1 << inode->i_blkbits;
	pgoff_t end_index = i_size_read(inode) >> PAGE_CACHE_SHIFT;
	zero_user(page, from, to-from);
	mark_page_accessed(page);
	if (page->index < end_index || !(mode & FALLOC_FL_KEEP_SIZE)) {
		if (!gfs2_is_writeback(ip))
			gfs2_page_add_databufs(ip, page, from, to);
		block_commit_write(page, from, to);
		return 0;
	}
	offset = 0;
	bh = page_buffers(page);
	while (offset < to) {
		if (offset >= from) {
			set_buffer_uptodate(bh);
			mark_buffer_dirty(bh);
			clear_buffer_new(bh);
			write_dirty_buffer(bh, WRITE);
		}
		offset += blksize;
		bh = bh->b_this_page;
	}
	offset = 0;
	bh = page_buffers(page);
	while (offset < to) {
		if (offset >= from) {
			wait_on_buffer(bh);
			if (!buffer_uptodate(bh))
				return -EIO;
		}
		offset += blksize;
		bh = bh->b_this_page;
	}
	return 0;
}
