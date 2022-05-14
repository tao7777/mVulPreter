static int prctl_set_vma_anon_name(unsigned long start, unsigned long end,
 unsigned long arg)
{
 unsigned long tmp;
 struct vm_area_struct * vma, *prev;
 int unmapped_error = 0;
 int error = -EINVAL;

 /*
	 * If the interval [start,end) covers some unmapped address
	 * ranges, just ignore them, but return -ENOMEM at the end.
	 * - this matches the handling in madvise.
	 */
	vma = find_vma_prev(current->mm, start, &prev);
 if (vma && start > vma->vm_start)
		prev = vma;

 for (;;) {
 /* Still start < end. */
		error = -ENOMEM;
 if (!vma)
 return error;

 /* Here start < (end|vma->vm_end). */
 if (start < vma->vm_start) {
			unmapped_error = -ENOMEM;
			start = vma->vm_start;
 if (start >= end)
 return error;
 }

 /* Here vma->vm_start <= start < (end|vma->vm_end) */
		tmp = vma->vm_end;
 if (end < tmp)

 			tmp = end;
 
 		/* Here vma->vm_start <= start < tmp <= (end|vma->vm_end). */
		error = prctl_update_vma_anon_name(vma, &prev, start, tmp,
 				(const char __user *)arg);
 		if (error)
 			return error;
		start = tmp;
 if (prev && start < prev->vm_end)
			start = prev->vm_end;
		error = unmapped_error;
 if (start >= end)
 return error;
 if (prev)
			vma = prev->vm_next;
 else /* madvise_remove dropped mmap_sem */
			vma = find_vma(current->mm, start);
 }
}
