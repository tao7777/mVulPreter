unsigned int munlock_vma_page(struct page *page)
{
 	unsigned int nr_pages;
 	struct zone *zone = page_zone(page);
 
 	BUG_ON(!PageLocked(page));
 
 	/*
	 * Serialize with any parallel __split_huge_page_refcount() which
	 * might otherwise copy PageMlocked to part of the tail pages before
	 * we clear it in the head page. It also stabilizes hpage_nr_pages().
	 */
	spin_lock_irq(&zone->lru_lock);

	nr_pages = hpage_nr_pages(page);
	if (!TestClearPageMlocked(page))
		goto unlock_out;

	__mod_zone_page_state(zone, NR_MLOCK, -nr_pages);

	if (__munlock_isolate_lru_page(page, true)) {
		spin_unlock_irq(&zone->lru_lock);
		__munlock_isolated_page(page);
		goto out;
	}
	__munlock_isolation_failed(page);

unlock_out:
	spin_unlock_irq(&zone->lru_lock);

out:
	return nr_pages - 1;
}
