static void freeary(struct ipc_namespace *ns, struct kern_ipc_perm *ipcp)
{
	struct sem_undo *un, *tu;
	struct sem_queue *q, *tq;
	struct sem_array *sma = container_of(ipcp, struct sem_array, sem_perm);
	struct list_head tasks;
	int i;

	/* Free the existing undo structures for this semaphore set.  */
	assert_spin_locked(&sma->sem_perm.lock);
	list_for_each_entry_safe(un, tu, &sma->list_id, list_id) {
		list_del(&un->list_id);
		spin_lock(&un->ulp->lock);
		un->semid = -1;
		list_del_rcu(&un->list_proc);
		spin_unlock(&un->ulp->lock);
		kfree_rcu(un, rcu);
	}

	/* Wake up all pending processes and let them fail with EIDRM. */
	INIT_LIST_HEAD(&tasks);
	list_for_each_entry_safe(q, tq, &sma->sem_pending, list) {
		unlink_queue(sma, q);
		wake_up_sem_queue_prepare(&tasks, q, -EIDRM);
	}
	for (i = 0; i < sma->sem_nsems; i++) {
		struct sem *sem = sma->sem_base + i;
		list_for_each_entry_safe(q, tq, &sem->sem_pending, list) {
			unlink_queue(sma, q);
			wake_up_sem_queue_prepare(&tasks, q, -EIDRM);
		}
	}
 
 	/* Remove the semaphore set from the IDR */
 	sem_rmid(ns, sma);
	sem_unlock(sma, -1);
 
 	wake_up_sem_queue_do(&tasks);
 	ns->used_sems -= sma->sem_nsems;
	security_sem_free(sma);
	ipc_rcu_putref(sma);
}
