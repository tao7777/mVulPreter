 static int semctl_main(struct ipc_namespace *ns, int semid, int semnum,
		int cmd, void __user *p)
{
	struct sem_array *sma;
	struct sem* curr;
	int err, nsems;
	ushort fast_sem_io[SEMMSL_FAST];
	ushort* sem_io = fast_sem_io;
	struct list_head tasks;

	INIT_LIST_HEAD(&tasks);

	rcu_read_lock();
	sma = sem_obtain_object_check(ns, semid);
	if (IS_ERR(sma)) {
		rcu_read_unlock();
		return PTR_ERR(sma);
	}

	nsems = sma->sem_nsems;

	err = -EACCES;
	if (ipcperms(ns, &sma->sem_perm,
			cmd == SETALL ? S_IWUGO : S_IRUGO)) {
		rcu_read_unlock();
		goto out_wakeup;
	}

	err = security_sem_semctl(sma, cmd);
	if (err) {
		rcu_read_unlock();
		goto out_wakeup;
	}

	err = -EACCES;
	switch (cmd) {
	case GETALL:
	{
		ushort __user *array = p;
		int i;

		if(nsems > SEMMSL_FAST) {
			sem_getref(sma);

			sem_io = ipc_alloc(sizeof(ushort)*nsems);
			if(sem_io == NULL) {
				sem_putref(sma);
				return -ENOMEM;
			}
 
 			sem_lock_and_putref(sma);
 			if (sma->sem_perm.deleted) {
				sem_unlock(sma, -1);
 				err = -EIDRM;
 				goto out_free;
 			}
		} else
			sem_lock(sma, NULL, -1);
 
 		for (i = 0; i < sma->sem_nsems; i++)
 			sem_io[i] = sma->sem_base[i].semval;
		sem_unlock(sma, -1);
 		err = 0;
 		if(copy_to_user(array, sem_io, nsems*sizeof(ushort)))
 			err = -EFAULT;
		goto out_free;
	}
	case SETALL:
	{
 		int i;
 		struct sem_undo *un;
 
		if (!ipc_rcu_getref(sma)) {
			rcu_read_unlock();
			return -EIDRM;
		}
 		rcu_read_unlock();
 
 		if(nsems > SEMMSL_FAST) {
			sem_io = ipc_alloc(sizeof(ushort)*nsems);
			if(sem_io == NULL) {
				sem_putref(sma);
				return -ENOMEM;
			}
		}

		if (copy_from_user (sem_io, p, nsems*sizeof(ushort))) {
			sem_putref(sma);
			err = -EFAULT;
			goto out_free;
		}

		for (i = 0; i < nsems; i++) {
			if (sem_io[i] > SEMVMX) {
				sem_putref(sma);
				err = -ERANGE;
				goto out_free;
			}
 		}
 		sem_lock_and_putref(sma);
 		if (sma->sem_perm.deleted) {
			sem_unlock(sma, -1);
 			err = -EIDRM;
 			goto out_free;
 		}

		for (i = 0; i < nsems; i++)
			sma->sem_base[i].semval = sem_io[i];

		assert_spin_locked(&sma->sem_perm.lock);
		list_for_each_entry(un, &sma->list_id, list_id) {
			for (i = 0; i < nsems; i++)
				un->semadj[i] = 0;
		}
		sma->sem_ctime = get_seconds();
		/* maybe some queued-up processes were waiting for this */
		do_smart_update(sma, NULL, 0, 0, &tasks);
		err = 0;
		goto out_unlock;
	}
	/* GETVAL, GETPID, GETNCTN, GETZCNT: fall-through */
	}
	err = -EINVAL;
	if (semnum < 0 || semnum >= nsems) {
		rcu_read_unlock();
 		goto out_wakeup;
 	}
 
	sem_lock(sma, NULL, -1);
 	curr = &sma->sem_base[semnum];
 
 	switch (cmd) {
	case GETVAL:
		err = curr->semval;
		goto out_unlock;
	case GETPID:
		err = curr->sempid;
		goto out_unlock;
	case GETNCNT:
		err = count_semncnt(sma,semnum);
		goto out_unlock;
	case GETZCNT:
		err = count_semzcnt(sma,semnum);
		goto out_unlock;
 	}
 
 out_unlock:
	sem_unlock(sma, -1);
 out_wakeup:
 	wake_up_sem_queue_do(&tasks);
 out_free:
	if(sem_io != fast_sem_io)
		ipc_free(sem_io, sizeof(ushort)*nsems);
	return err;
}
