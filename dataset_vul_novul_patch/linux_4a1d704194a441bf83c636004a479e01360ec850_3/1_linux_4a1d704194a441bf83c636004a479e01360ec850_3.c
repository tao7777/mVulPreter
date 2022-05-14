static int smaps_pte_range(pmd_t *pmd, unsigned long addr, unsigned long end,
			   struct mm_walk *walk)
{
	struct mem_size_stats *mss = walk->private;
	struct vm_area_struct *vma = mss->vma;
	pte_t *pte;
	spinlock_t *ptl;

	spin_lock(&walk->mm->page_table_lock);
	if (pmd_trans_huge(*pmd)) {
		if (pmd_trans_splitting(*pmd)) {
			spin_unlock(&walk->mm->page_table_lock);
			wait_split_huge_page(vma->anon_vma, pmd);
		} else {
			smaps_pte_entry(*(pte_t *)pmd, addr,
					HPAGE_PMD_SIZE, walk);
			spin_unlock(&walk->mm->page_table_lock);
			mss->anonymous_thp += HPAGE_PMD_SIZE;
			return 0;
		}
 	} else {
 		spin_unlock(&walk->mm->page_table_lock);
 	}
 	/*
 	 * The mmap_sem held all the way back in m_start() is what
 	 * keeps khugepaged out of here and from collapsing things
	 * in here.
	 */
	pte = pte_offset_map_lock(vma->vm_mm, pmd, addr, &ptl);
	for (; addr != end; pte++, addr += PAGE_SIZE)
		smaps_pte_entry(*pte, addr, PAGE_SIZE, walk);
	pte_unmap_unlock(pte - 1, ptl);
	cond_resched();
	return 0;
}
