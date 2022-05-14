static unsigned char mincore_page(struct address_space *mapping, pgoff_t pgoff)
{
	unsigned char present = 0;
	struct page *page;
	/*
	 * When tmpfs swaps out a page from a file, any process mapping that
	 * file will not get a swp_entry_t in its pte, but rather it is like
	 * any other file mapping (ie. marked !present and faulted in with
	 * tmpfs's .fault). So swapped out tmpfs mappings are tested here.
	 */
#ifdef CONFIG_SWAP
	if (shmem_mapping(mapping)) {
		page = find_get_entry(mapping, pgoff);
		/*
		 * shmem/tmpfs may return swap: account for swapcache
		 * page too.
		 */
		if (xa_is_value(page)) {
			swp_entry_t swp = radix_to_swp_entry(page);
			page = find_get_page(swap_address_space(swp),
					     swp_offset(swp));
		}
	} else
		page = find_get_page(mapping, pgoff);
#else
	page = find_get_page(mapping, pgoff);
#endif
	if (page) {
		present = PageUptodate(page);
		put_page(page);
	}
	return present;
}
