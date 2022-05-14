static struct sem_undo *find_alloc_undo(struct ipc_namespace *ns, int semid)
{
 	struct sem_array *sma;
 	struct sem_undo_list *ulp;
 	struct sem_undo *un, *new;
	int nsems, error;
 
 	error = get_undo_list(&ulp);
 	if (error)
		return ERR_PTR(error);

	rcu_read_lock();
	spin_lock(&ulp->lock);
	un = lookup_undo(ulp, semid);
	spin_unlock(&ulp->lock);
	if (likely(un!=NULL))
		goto out;

	/* no undo structure around - allocate one. */
	/* step 1: figure out the size of the semaphore array */
	sma = sem_obtain_object_check(ns, semid);
	if (IS_ERR(sma)) {
		rcu_read_unlock();
		return ERR_CAST(sma);
 	}
 
 	nsems = sma->sem_nsems;
	if (!ipc_rcu_getref(sma)) {
		rcu_read_unlock();
		un = ERR_PTR(-EIDRM);
		goto out;
	}
 	rcu_read_unlock();
 
 	/* step 2: allocate new undo structure */
	new = kzalloc(sizeof(struct sem_undo) + sizeof(short)*nsems, GFP_KERNEL);
	if (!new) {
		sem_putref(sma);
		return ERR_PTR(-ENOMEM);
	}

 	/* step 3: Acquire the lock on semaphore array */
 	sem_lock_and_putref(sma);
 	if (sma->sem_perm.deleted) {
		sem_unlock(sma, -1);
 		kfree(new);
 		un = ERR_PTR(-EIDRM);
 		goto out;
	}
	spin_lock(&ulp->lock);

	/*
	 * step 4: check for races: did someone else allocate the undo struct?
	 */
	un = lookup_undo(ulp, semid);
	if (un) {
		kfree(new);
		goto success;
	}
	/* step 5: initialize & link new undo structure */
	new->semadj = (short *) &new[1];
	new->ulp = ulp;
	new->semid = semid;
	assert_spin_locked(&ulp->lock);
	list_add_rcu(&new->list_proc, &ulp->list_proc);
	assert_spin_locked(&sma->sem_perm.lock);
	list_add(&new->list_id, &sma->list_id);
	un = new;

 success:
 	spin_unlock(&ulp->lock);
 	rcu_read_lock();
	sem_unlock(sma, -1);
 out:
 	return un;
 }
