void exit_sem(struct task_struct *tsk)
{
	struct sem_undo_list *ulp;

	ulp = tsk->sysvsem.undo_list;
	if (!ulp)
		return;
	tsk->sysvsem.undo_list = NULL;

	if (!atomic_dec_and_test(&ulp->refcnt))
		return;

	for (;;) {
 		struct sem_array *sma;
 		struct sem_undo *un;
 		struct list_head tasks;
		int semid, i;
 
 		rcu_read_lock();
 		un = list_entry_rcu(ulp->list_proc.next,
				    struct sem_undo, list_proc);
		if (&un->list_proc == &ulp->list_proc)
 			semid = -1;
 		 else
 			semid = un->semid;
 
		if (semid == -1) {
			rcu_read_unlock();
 			break;
		}
 
		sma = sem_obtain_object_check(tsk->nsproxy->ipc_ns, un->semid);
 		/* exit_sem raced with IPC_RMID, nothing to do */
		if (IS_ERR(sma)) {
			rcu_read_unlock();
 			continue;
		}
 
		sem_lock(sma, NULL, -1);
 		un = __lookup_undo(ulp, semid);
 		if (un == NULL) {
 			/* exit_sem raced with IPC_RMID+semget() that created
 			 * exactly the same semid. Nothing to do.
 			 */
			sem_unlock(sma, -1);
 			continue;
 		}
 
		/* remove un from the linked lists */
		assert_spin_locked(&sma->sem_perm.lock);
		list_del(&un->list_id);

		spin_lock(&ulp->lock);
		list_del_rcu(&un->list_proc);
		spin_unlock(&ulp->lock);

		/* perform adjustments registered in un */
		for (i = 0; i < sma->sem_nsems; i++) {
			struct sem * semaphore = &sma->sem_base[i];
			if (un->semadj[i]) {
				semaphore->semval += un->semadj[i];
				/*
				 * Range checks of the new semaphore value,
				 * not defined by sus:
				 * - Some unices ignore the undo entirely
				 *   (e.g. HP UX 11i 11.22, Tru64 V5.1)
				 * - some cap the value (e.g. FreeBSD caps
				 *   at 0, but doesn't enforce SEMVMX)
				 *
				 * Linux caps the semaphore value, both at 0
				 * and at SEMVMX.
				 *
				 * 	Manfred <manfred@colorfullife.com>
				 */
				if (semaphore->semval < 0)
					semaphore->semval = 0;
				if (semaphore->semval > SEMVMX)
					semaphore->semval = SEMVMX;
				semaphore->sempid = task_tgid_vnr(current);
			}
		}
 		/* maybe some queued-up processes were waiting for this */
 		INIT_LIST_HEAD(&tasks);
 		do_smart_update(sma, NULL, 0, 1, &tasks);
		sem_unlock(sma, -1);
 		wake_up_sem_queue_do(&tasks);
 
 		kfree_rcu(un, rcu);
	}
	kfree(ulp);
}
