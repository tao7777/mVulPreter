static inline int do_exception(struct pt_regs *regs, int access,
			       unsigned long trans_exc_code)
{
	struct task_struct *tsk;
	struct mm_struct *mm;
	struct vm_area_struct *vma;
	unsigned long address;
	unsigned int flags;
	int fault;

	if (notify_page_fault(regs))
		return 0;

	tsk = current;
	mm = tsk->mm;

	/*
	 * Verify that the fault happened in user space, that
	 * we are not in an interrupt and that there is a 
	 * user context.
	 */
	fault = VM_FAULT_BADCONTEXT;
	if (unlikely(!user_space_fault(trans_exc_code) || in_atomic() || !mm))
 		goto out;
 
 	address = trans_exc_code & __FAIL_ADDR_MASK;
	perf_sw_event(PERF_COUNT_SW_PAGE_FAULTS, 1, regs, address);
 	flags = FAULT_FLAG_ALLOW_RETRY;
 	if (access == VM_WRITE || (trans_exc_code & store_indication) == 0x400)
 		flags |= FAULT_FLAG_WRITE;
retry:
	down_read(&mm->mmap_sem);

	fault = VM_FAULT_BADMAP;
	vma = find_vma(mm, address);
	if (!vma)
		goto out_up;

	if (unlikely(vma->vm_start > address)) {
		if (!(vma->vm_flags & VM_GROWSDOWN))
			goto out_up;
		if (expand_stack(vma, address))
			goto out_up;
	}

	/*
	 * Ok, we have a good vm_area for this memory access, so
	 * we can handle it..
	 */
	fault = VM_FAULT_BADACCESS;
	if (unlikely(!(vma->vm_flags & access)))
		goto out_up;

	if (is_vm_hugetlb_page(vma))
		address &= HPAGE_MASK;
	/*
	 * If for any reason at all we couldn't handle the fault,
	 * make sure we exit gracefully rather than endlessly redo
	 * the fault.
	 */
	fault = handle_mm_fault(mm, vma, address, flags);
	if (unlikely(fault & VM_FAULT_ERROR))
		goto out_up;

	/*
	 * Major/minor page fault accounting is only done on the
	 * initial attempt. If we go through a retry, it is extremely
	 * likely that the page will be found in page cache at that point.
	 */
 	if (flags & FAULT_FLAG_ALLOW_RETRY) {
 		if (fault & VM_FAULT_MAJOR) {
 			tsk->maj_flt++;
			perf_sw_event(PERF_COUNT_SW_PAGE_FAULTS_MAJ, 1,
 				      regs, address);
 		} else {
 			tsk->min_flt++;
			perf_sw_event(PERF_COUNT_SW_PAGE_FAULTS_MIN, 1,
 				      regs, address);
 		}
 		if (fault & VM_FAULT_RETRY) {
			/* Clear FAULT_FLAG_ALLOW_RETRY to avoid any risk
			 * of starvation. */
			flags &= ~FAULT_FLAG_ALLOW_RETRY;
			goto retry;
		}
	}
	/*
	 * The instruction that caused the program check will
	 * be repeated. Don't signal single step via SIGTRAP.
	 */
	clear_tsk_thread_flag(tsk, TIF_PER_TRAP);
	fault = 0;
out_up:
	up_read(&mm->mmap_sem);
out:
	return fault;
}
