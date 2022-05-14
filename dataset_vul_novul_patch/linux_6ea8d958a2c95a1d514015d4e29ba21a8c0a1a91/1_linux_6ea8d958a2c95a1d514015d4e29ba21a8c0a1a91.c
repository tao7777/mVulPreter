static long madvise_willneed(struct vm_area_struct *vma,
			     struct vm_area_struct **prev,
			     unsigned long start, unsigned long end)
 {
 	struct file *file = vma->vm_file;
 
 #ifdef CONFIG_SWAP
 	if (!file) {
		*prev = vma;
 		force_swapin_readahead(vma, start, end);
 		return 0;
 	}
 
 	if (shmem_mapping(file->f_mapping)) {
		*prev = vma;
 		force_shm_swapin_readahead(vma, start, end,
 					file->f_mapping);
 		return 0;
	}
#else
	if (!file)
		return -EBADF;
#endif

	if (IS_DAX(file_inode(file))) {
		/* no bad return value, but ignore advice */
 		return 0;
 	}
 
	*prev = vma;
 	start = ((start - vma->vm_start) >> PAGE_SHIFT) + vma->vm_pgoff;
 	if (end > vma->vm_end)
 		end = vma->vm_end;
	end = ((end - vma->vm_start) >> PAGE_SHIFT) + vma->vm_pgoff;

	force_page_cache_readahead(file->f_mapping, file, start, end - start);
	return 0;
}
