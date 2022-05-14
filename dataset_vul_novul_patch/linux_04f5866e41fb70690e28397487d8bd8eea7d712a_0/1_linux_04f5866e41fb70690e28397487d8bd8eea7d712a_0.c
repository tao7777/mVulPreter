static ssize_t clear_refs_write(struct file *file, const char __user *buf,
				size_t count, loff_t *ppos)
{
	struct task_struct *task;
	char buffer[PROC_NUMBUF];
	struct mm_struct *mm;
	struct vm_area_struct *vma;
	enum clear_refs_types type;
	struct mmu_gather tlb;
	int itype;
	int rv;

	memset(buffer, 0, sizeof(buffer));
	if (count > sizeof(buffer) - 1)
		count = sizeof(buffer) - 1;
	if (copy_from_user(buffer, buf, count))
		return -EFAULT;
	rv = kstrtoint(strstrip(buffer), 10, &itype);
	if (rv < 0)
		return rv;
	type = (enum clear_refs_types)itype;
	if (type < CLEAR_REFS_ALL || type >= CLEAR_REFS_LAST)
		return -EINVAL;

	task = get_proc_task(file_inode(file));
	if (!task)
		return -ESRCH;
	mm = get_task_mm(task);
	if (mm) {
		struct mmu_notifier_range range;
		struct clear_refs_private cp = {
			.type = type,
		};
		struct mm_walk clear_refs_walk = {
			.pmd_entry = clear_refs_pte_range,
			.test_walk = clear_refs_test_walk,
			.mm = mm,
			.private = &cp,
		};

		if (type == CLEAR_REFS_MM_HIWATER_RSS) {
			if (down_write_killable(&mm->mmap_sem)) {
				count = -EINTR;
				goto out_mm;
			}

			/*
			 * Writing 5 to /proc/pid/clear_refs resets the peak
			 * resident set size to this mm's current rss value.
			 */
			reset_mm_hiwater_rss(mm);
			up_write(&mm->mmap_sem);
			goto out_mm;
		}

		down_read(&mm->mmap_sem);
		tlb_gather_mmu(&tlb, mm, 0, -1);
		if (type == CLEAR_REFS_SOFT_DIRTY) {
			for (vma = mm->mmap; vma; vma = vma->vm_next) {
				if (!(vma->vm_flags & VM_SOFTDIRTY))
					continue;
				up_read(&mm->mmap_sem);
				if (down_write_killable(&mm->mmap_sem)) {
 					count = -EINTR;
 					goto out_mm;
 				}
 				for (vma = mm->mmap; vma; vma = vma->vm_next) {
 					vma->vm_flags &= ~VM_SOFTDIRTY;
 					vma_set_page_prot(vma);
				}
				downgrade_write(&mm->mmap_sem);
				break;
			}

			mmu_notifier_range_init(&range, mm, 0, -1UL);
			mmu_notifier_invalidate_range_start(&range);
		}
		walk_page_range(0, mm->highest_vm_end, &clear_refs_walk);
		if (type == CLEAR_REFS_SOFT_DIRTY)
			mmu_notifier_invalidate_range_end(&range);
		tlb_finish_mmu(&tlb, 0, -1);
		up_read(&mm->mmap_sem);
out_mm:
		mmput(mm);
	}
	put_task_struct(task);

	return count;
}
