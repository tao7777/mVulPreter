static int pagemap_pte_range(pmd_t *pmd, unsigned long addr, unsigned long end,
			     struct mm_walk *walk)
{
	struct vm_area_struct *vma;
	struct pagemapread *pm = walk->private;
	pte_t *pte;
 	int err = 0;
 
 	split_huge_page_pmd(walk->mm, pmd);
 
 	/* find the first VMA at or above 'addr' */
 	vma = find_vma(walk->mm, addr);
	for (; addr != end; addr += PAGE_SIZE) {
		u64 pfn = PM_NOT_PRESENT;

		/* check to see if we've left 'vma' behind
		 * and need a new, higher one */
		if (vma && (addr >= vma->vm_end))
			vma = find_vma(walk->mm, addr);

		/* check that 'vma' actually covers this address,
		 * and that it isn't a huge page vma */
		if (vma && (vma->vm_start <= addr) &&
		    !is_vm_hugetlb_page(vma)) {
			pte = pte_offset_map(pmd, addr);
			pfn = pte_to_pagemap_entry(*pte);
			/* unmap before userspace copy */
			pte_unmap(pte);
		}
		err = add_to_pagemap(addr, pfn, pm);
		if (err)
			return err;
	}

	cond_resched();

	return err;
}
