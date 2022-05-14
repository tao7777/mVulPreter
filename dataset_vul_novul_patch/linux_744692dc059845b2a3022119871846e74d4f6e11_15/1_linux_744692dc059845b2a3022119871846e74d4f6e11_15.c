static void mpage_put_bnr_to_bhs(struct mpage_da_data *mpd, sector_t logical,
				 struct buffer_head *exbh)
{
	struct inode *inode = mpd->inode;
	struct address_space *mapping = inode->i_mapping;
	int blocks = exbh->b_size >> inode->i_blkbits;
	sector_t pblock = exbh->b_blocknr, cur_logical;
	struct buffer_head *head, *bh;
	pgoff_t index, end;
	struct pagevec pvec;
	int nr_pages, i;

	index = logical >> (PAGE_CACHE_SHIFT - inode->i_blkbits);
	end = (logical + blocks - 1) >> (PAGE_CACHE_SHIFT - inode->i_blkbits);
	cur_logical = index << (PAGE_CACHE_SHIFT - inode->i_blkbits);

	pagevec_init(&pvec, 0);

	while (index <= end) {
		/* XXX: optimize tail */
		nr_pages = pagevec_lookup(&pvec, mapping, index, PAGEVEC_SIZE);
		if (nr_pages == 0)
			break;
		for (i = 0; i < nr_pages; i++) {
			struct page *page = pvec.pages[i];

			index = page->index;
			if (index > end)
				break;
			index++;

			BUG_ON(!PageLocked(page));
			BUG_ON(PageWriteback(page));
			BUG_ON(!page_has_buffers(page));

			bh = page_buffers(page);
			head = bh;

			/* skip blocks out of the range */
			do {
				if (cur_logical >= logical)
					break;
				cur_logical++;
			} while ((bh = bh->b_this_page) != head);

			do {
				if (cur_logical >= logical + blocks)
					break;

				if (buffer_delay(bh) ||
						buffer_unwritten(bh)) {

					BUG_ON(bh->b_bdev != inode->i_sb->s_bdev);

					if (buffer_delay(bh)) {
						clear_buffer_delay(bh);
						bh->b_blocknr = pblock;
					} else {
						/*
						 * unwritten already should have
						 * blocknr assigned. Verify that
						 */
						clear_buffer_unwritten(bh);
						BUG_ON(bh->b_blocknr != pblock);
					}

 				} else if (buffer_mapped(bh))
 					BUG_ON(bh->b_blocknr != pblock);
 
 				cur_logical++;
 				pblock++;
 			} while ((bh = bh->b_this_page) != head);
		}
		pagevec_release(&pvec);
	}
}
