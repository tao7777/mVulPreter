 static int ext4_dax_mkwrite(struct vm_area_struct *vma, struct vm_fault *vmf)
 {
	int err;
	struct inode *inode = file_inode(vma->vm_file);

	sb_start_pagefault(inode->i_sb);
	file_update_time(vma->vm_file);
	down_read(&EXT4_I(inode)->i_mmap_sem);
	err = __dax_mkwrite(vma, vmf, ext4_get_block_dax,
			    ext4_end_io_unwritten);
	up_read(&EXT4_I(inode)->i_mmap_sem);
	sb_end_pagefault(inode->i_sb);

	return err;
}

/*
 * Handle write fault for VM_MIXEDMAP mappings. Similarly to ext4_dax_mkwrite()
 * handler we check for races agaist truncate. Note that since we cycle through
 * i_mmap_sem, we are sure that also any hole punching that began before we
 * were called is finished by now and so if it included part of the file we
 * are working on, our pte will get unmapped and the check for pte_same() in
 * wp_pfn_shared() fails. Thus fault gets retried and things work out as
 * desired.
 */
static int ext4_dax_pfn_mkwrite(struct vm_area_struct *vma,
				struct vm_fault *vmf)
{
	struct inode *inode = file_inode(vma->vm_file);
	struct super_block *sb = inode->i_sb;
	int ret = VM_FAULT_NOPAGE;
	loff_t size;

	sb_start_pagefault(sb);
	file_update_time(vma->vm_file);
	down_read(&EXT4_I(inode)->i_mmap_sem);
	size = (i_size_read(inode) + PAGE_SIZE - 1) >> PAGE_SHIFT;
	if (vmf->pgoff >= size)
		ret = VM_FAULT_SIGBUS;
	up_read(&EXT4_I(inode)->i_mmap_sem);
	sb_end_pagefault(sb);

	return ret;
 }
