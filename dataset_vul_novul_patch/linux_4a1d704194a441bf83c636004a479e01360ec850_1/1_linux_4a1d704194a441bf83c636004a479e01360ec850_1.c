static int gather_pte_stats(pmd_t *pmd, unsigned long addr,
		unsigned long end, struct mm_walk *walk)
{
	struct numa_maps *md;
	spinlock_t *ptl;
	pte_t *orig_pte;
	pte_t *pte;

	md = walk->private;
	spin_lock(&walk->mm->page_table_lock);
	if (pmd_trans_huge(*pmd)) {
		if (pmd_trans_splitting(*pmd)) {
			spin_unlock(&walk->mm->page_table_lock);
			wait_split_huge_page(md->vma->anon_vma, pmd);
		} else {
			pte_t huge_pte = *(pte_t *)pmd;
			struct page *page;

			page = can_gather_numa_stats(huge_pte, md->vma, addr);
			if (page)
				gather_stats(page, md, pte_dirty(huge_pte),
						HPAGE_PMD_SIZE/PAGE_SIZE);
			spin_unlock(&walk->mm->page_table_lock);
			return 0;
		}
	} else {
 		spin_unlock(&walk->mm->page_table_lock);
 	}
 
 	orig_pte = pte = pte_offset_map_lock(walk->mm, pmd, addr, &ptl);
 	do {
 		struct page *page = can_gather_numa_stats(*pte, md->vma, addr);
		if (!page)
			continue;
		gather_stats(page, md, pte_dirty(*pte), 1);

	} while (pte++, addr += PAGE_SIZE, addr != end);
	pte_unmap_unlock(orig_pte, ptl);
	return 0;
}
