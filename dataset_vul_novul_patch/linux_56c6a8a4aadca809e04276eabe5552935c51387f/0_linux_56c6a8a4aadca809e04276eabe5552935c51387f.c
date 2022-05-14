 unsigned int oom_badness(struct task_struct *p, struct mem_cgroup *mem,
 		      const nodemask_t *nodemask, unsigned long totalpages)
 {
	long points;
 
 	if (oom_unkillable_task(p, mem, nodemask))
 		return 0;

	p = find_lock_task_mm(p);
	if (!p)
		return 0;

	/*
	 * Shortcut check for a thread sharing p->mm that is OOM_SCORE_ADJ_MIN
	 * so the entire heuristic doesn't need to be executed for something
	 * that cannot be killed.
	 */
	if (atomic_read(&p->mm->oom_disable_count)) {
		task_unlock(p);
		return 0;
	}

	/*
	 * The memory controller may have a limit of 0 bytes, so avoid a divide
	 * by zero, if necessary.
	 */
	if (!totalpages)
		totalpages = 1;

	/*
	 * The baseline for the badness score is the proportion of RAM that each
	 * task's rss, pagetable and swap space use.
	 */
	points = get_mm_rss(p->mm) + p->mm->nr_ptes;
	points += get_mm_counter(p->mm, MM_SWAPENTS);

	points *= 1000;
	points /= totalpages;
	task_unlock(p);

	/*
	 * Root processes get 3% bonus, just like the __vm_enough_memory()
	 * implementation used by LSMs.
	 */
	if (has_capability_noaudit(p, CAP_SYS_ADMIN))
		points -= 30;

	/*
	 * /proc/pid/oom_score_adj ranges from -1000 to +1000 such that it may
	 * either completely disable oom killing or always prefer a certain
	 * task.
	 */
	points += p->signal->oom_score_adj;

	/*
	 * Never return 0 for an eligible task that may be killed since it's
	 * possible that no single user task uses more than 0.1% of memory and
	 * no single admin tasks uses more than 3.0%.
	 */
	if (points <= 0)
		return 1;
	return (points < 1000) ? points : 1000;
}
