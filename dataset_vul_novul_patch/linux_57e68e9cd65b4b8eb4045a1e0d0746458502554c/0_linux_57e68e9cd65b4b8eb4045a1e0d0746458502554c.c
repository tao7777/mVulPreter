 void mlock_vma_page(struct page *page)
 {
	/* Serialize with page migration */
 	BUG_ON(!PageLocked(page));
 
 	if (!TestSetPageMlocked(page)) {
		mod_zone_page_state(page_zone(page), NR_MLOCK,
				    hpage_nr_pages(page));
		count_vm_event(UNEVICTABLE_PGMLOCKED);
		if (!isolate_lru_page(page))
			putback_lru_page(page);
	}
}
