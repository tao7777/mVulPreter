void vmacache_flush_all(struct mm_struct *mm)
{
	struct task_struct *g, *p;
	count_vm_vmacache_event(VMACACHE_FULL_FLUSHES);
	/*
	 * Single threaded tasks need not iterate the entire
	 * list of process. We can avoid the flushing as well
	 * since the mm's seqnum was increased and don't have
	 * to worry about other threads' seqnum. Current's
	 * flush will occur upon the next lookup.
	 */
	if (atomic_read(&mm->mm_users) == 1)
		return;
	rcu_read_lock();
	for_each_process_thread(g, p) {
		/*
		 * Only flush the vmacache pointers as the
		 * mm seqnum is already set and curr's will
		 * be set upon invalidation when the next
		 * lookup is done.
		 */
		if (mm == p->mm)
			vmacache_flush(p);
	}
	rcu_read_unlock();
}
