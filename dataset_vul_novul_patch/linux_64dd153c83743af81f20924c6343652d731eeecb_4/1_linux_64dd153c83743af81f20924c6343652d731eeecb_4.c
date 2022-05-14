static int write_empty_blocks(struct page *page, unsigned from, unsigned to,
			      int mode)
{
	struct inode *inode = page->mapping->host;
	unsigned start, end, next, blksize;
	sector_t block = page->index << (PAGE_CACHE_SHIFT - inode->i_blkbits);
	int ret;
	blksize = 1 << inode->i_blkbits;
	next = end = 0;
	while (next < from) {
		next += blksize;
		block++;
	}
	start = next;
	do {
		next += blksize;
		ret = needs_empty_write(block, inode);
		if (unlikely(ret < 0))
			return ret;
		if (ret == 0) {
			if (end) {
				ret = __block_write_begin(page, start, end - start,
							  gfs2_block_map);
				if (unlikely(ret))
					return ret;
				ret = empty_write_end(page, start, end, mode);
				if (unlikely(ret))
					return ret;
				end = 0;
			}
			start = next;
		}
		else
			end = next;
		block++;
	} while (next < to);
	if (end) {
		ret = __block_write_begin(page, start, end - start, gfs2_block_map);
		if (unlikely(ret))
			return ret;
		ret = empty_write_end(page, start, end, mode);
		if (unlikely(ret))
			return ret;
	}
	return 0;
}
