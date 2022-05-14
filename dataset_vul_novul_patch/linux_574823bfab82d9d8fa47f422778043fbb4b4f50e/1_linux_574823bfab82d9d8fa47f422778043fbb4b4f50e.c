static int __mincore_unmapped_range(unsigned long addr, unsigned long end,
				struct vm_area_struct *vma, unsigned char *vec)
{
	unsigned long nr = (end - addr) >> PAGE_SHIFT;
	int i;
	if (vma->vm_file) {
		pgoff_t pgoff;
		pgoff = linear_page_index(vma, addr);
		for (i = 0; i < nr; i++, pgoff++)
			vec[i] = mincore_page(vma->vm_file->f_mapping, pgoff);
	} else {
		for (i = 0; i < nr; i++)
			vec[i] = 0;
	}
	return nr;
}
