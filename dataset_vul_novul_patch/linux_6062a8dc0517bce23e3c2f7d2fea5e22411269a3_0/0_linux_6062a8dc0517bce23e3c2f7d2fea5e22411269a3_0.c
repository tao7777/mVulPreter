SYSCALL_DEFINE4(semtimedop, int, semid, struct sembuf __user *, tsops,
		unsigned, nsops, const struct timespec __user *, timeout)
{
	int error = -EINVAL;
	struct sem_array *sma;
 	struct sembuf fast_sops[SEMOPM_FAST];
 	struct sembuf* sops = fast_sops, *sop;
 	struct sem_undo *un;
	int undos = 0, alter = 0, max, locknum;
 	struct sem_queue queue;
 	unsigned long jiffies_left = 0;
 	struct ipc_namespace *ns;
	struct list_head tasks;

	ns = current->nsproxy->ipc_ns;

	if (nsops < 1 || semid < 0)
		return -EINVAL;
	if (nsops > ns->sc_semopm)
		return -E2BIG;
	if(nsops > SEMOPM_FAST) {
		sops = kmalloc(sizeof(*sops)*nsops,GFP_KERNEL);
		if(sops==NULL)
			return -ENOMEM;
	}
	if (copy_from_user (sops, tsops, nsops * sizeof(*tsops))) {
		error=-EFAULT;
		goto out_free;
	}
	if (timeout) {
		struct timespec _timeout;
		if (copy_from_user(&_timeout, timeout, sizeof(*timeout))) {
			error = -EFAULT;
			goto out_free;
		}
		if (_timeout.tv_sec < 0 || _timeout.tv_nsec < 0 ||
			_timeout.tv_nsec >= 1000000000L) {
			error = -EINVAL;
			goto out_free;
		}
		jiffies_left = timespec_to_jiffies(&_timeout);
	}
	max = 0;
	for (sop = sops; sop < sops + nsops; sop++) {
		if (sop->sem_num >= max)
			max = sop->sem_num;
		if (sop->sem_flg & SEM_UNDO)
			undos = 1;
		if (sop->sem_op != 0)
 			alter = 1;
 	}
 
	INIT_LIST_HEAD(&tasks);

 	if (undos) {
		/* On success, find_alloc_undo takes the rcu_read_lock */
 		un = find_alloc_undo(ns, semid);
 		if (IS_ERR(un)) {
 			error = PTR_ERR(un);
 			goto out_free;
 		}
	} else {
 		un = NULL;
		rcu_read_lock();
	}
 
 	sma = sem_obtain_object_check(ns, semid);
 	if (IS_ERR(sma)) {
		rcu_read_unlock();
 		error = PTR_ERR(sma);
 		goto out_free;
 	}

	error = -EFBIG;
	if (max >= sma->sem_nsems) {
		rcu_read_unlock();
		goto out_wakeup;
	}

	error = -EACCES;
	if (ipcperms(ns, &sma->sem_perm, alter ? S_IWUGO : S_IRUGO)) {
		rcu_read_unlock();
		goto out_wakeup;
	}

	error = security_sem_semop(sma, sops, nsops, alter);
	if (error) {
		rcu_read_unlock();
		goto out_wakeup;
	}

	/*
	 * semid identifiers are not unique - find_alloc_undo may have
	 * allocated an undo structure, it was invalidated by an RMID
	 * and now a new array with received the same id. Check and fail.
	 * This case can be detected checking un->semid. The existence of
 	 * "un" itself is guaranteed by rcu.
 	 */
 	error = -EIDRM;
	locknum = sem_lock(sma, sops, nsops);
	if (un && un->semid == -1)
		goto out_unlock_free;
 
 	error = try_atomic_semop (sma, sops, nsops, un, task_tgid_vnr(current));
 	if (error <= 0) {
		if (alter && error == 0)
			do_smart_update(sma, sops, nsops, 1, &tasks);

		goto out_unlock_free;
	}

	/* We need to sleep on this operation, so we put the current
	 * task into the pending queue and go to sleep.
	 */
		
	queue.sops = sops;
	queue.nsops = nsops;
	queue.undo = un;
	queue.pid = task_tgid_vnr(current);
	queue.alter = alter;

	if (nsops == 1) {
		struct sem *curr;
		curr = &sma->sem_base[sops->sem_num];

		if (alter)
			list_add_tail(&queue.list, &curr->sem_pending);
		else
			list_add(&queue.list, &curr->sem_pending);
	} else {
		if (alter)
			list_add_tail(&queue.list, &sma->sem_pending);
		else
			list_add(&queue.list, &sma->sem_pending);
		sma->complex_count++;
	}

	queue.status = -EINTR;
	queue.sleeper = current;
 
 sleep_again:
 	current->state = TASK_INTERRUPTIBLE;
	sem_unlock(sma, locknum);
 
 	if (timeout)
 		jiffies_left = schedule_timeout(jiffies_left);
	else
		schedule();

	error = get_queue_result(&queue);

	if (error != -EINTR) {
		/* fast path: update_queue already obtained all requested
		 * resources.
		 * Perform a smp_mb(): User space could assume that semop()
		 * is a memory barrier: Without the mb(), the cpu could
		 * speculatively read in user space stale data that was
		 * overwritten by the previous owner of the semaphore.
		 */
		smp_mb();

 		goto out_free;
 	}
 
	sma = sem_obtain_lock(ns, semid, sops, nsops, &locknum);
 
 	/*
 	 * Wait until it's guaranteed that no wakeup_sem_queue_do() is ongoing.
	 */
	error = get_queue_result(&queue);

	/*
	 * Array removed? If yes, leave without sem_unlock().
	 */
	if (IS_ERR(sma)) {
		goto out_free;
	}


	/*
	 * If queue.status != -EINTR we are woken up by another process.
	 * Leave without unlink_queue(), but with sem_unlock().
	 */

	if (error != -EINTR) {
		goto out_unlock_free;
	}

	/*
	 * If an interrupt occurred we have to clean up the queue
	 */
	if (timeout && jiffies_left == 0)
		error = -EAGAIN;

	/*
	 * If the wakeup was spurious, just retry
	 */
	if (error == -EINTR && !signal_pending(current))
		goto sleep_again;

 	unlink_queue(sma, &queue);
 
 out_unlock_free:
	sem_unlock(sma, locknum);
 out_wakeup:
 	wake_up_sem_queue_do(&tasks);
 out_free:
	if(sops != fast_sops)
		kfree(sops);
	return error;
}
