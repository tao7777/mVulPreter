static void mark_screen_rdonly(struct mm_struct *mm)
{
	pgd_t *pgd;
	pud_t *pud;
	pmd_t *pmd;
	pte_t *pte;
 	spinlock_t *ptl;
 	int i;
 
	down_write(&mm->mmap_sem);
 	pgd = pgd_offset(mm, 0xA0000);
 	if (pgd_none_or_clear_bad(pgd))
 		goto out;
	pud = pud_offset(pgd, 0xA0000);
	if (pud_none_or_clear_bad(pud))
		goto out;
	pmd = pmd_offset(pud, 0xA0000);
	split_huge_page_pmd(mm, pmd);
	if (pmd_none_or_clear_bad(pmd))
		goto out;
	pte = pte_offset_map_lock(mm, pmd, 0xA0000, &ptl);
	for (i = 0; i < 32; i++) {
		if (pte_present(*pte))
			set_pte(pte, pte_wrprotect(*pte));
		pte++;
 	}
 	pte_unmap_unlock(pte, ptl);
 out:
	up_write(&mm->mmap_sem);
 	flush_tlb();
 }
