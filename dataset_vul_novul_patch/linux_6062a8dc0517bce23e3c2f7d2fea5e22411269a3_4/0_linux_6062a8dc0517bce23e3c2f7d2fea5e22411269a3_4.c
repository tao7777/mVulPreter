static int newary(struct ipc_namespace *ns, struct ipc_params *params)
{
	int id;
	int retval;
	struct sem_array *sma;
	int size;
	key_t key = params->key;
	int nsems = params->u.nsems;
	int semflg = params->flg;
	int i;

	if (!nsems)
		return -EINVAL;
	if (ns->used_sems + nsems > ns->sc_semmns)
		return -ENOSPC;

	size = sizeof (*sma) + nsems * sizeof (struct sem);
	sma = ipc_rcu_alloc(size);
	if (!sma) {
		return -ENOMEM;
	}
	memset (sma, 0, size);

	sma->sem_perm.mode = (semflg & S_IRWXUGO);
	sma->sem_perm.key = key;

	sma->sem_perm.security = NULL;
	retval = security_sem_alloc(sma);
	if (retval) {
		ipc_rcu_putref(sma);
		return retval;
	}

	id = ipc_addid(&sem_ids(ns), &sma->sem_perm, ns->sc_semmni);
	if (id < 0) {
		security_sem_free(sma);
		ipc_rcu_putref(sma);
		return id;
	}
	ns->used_sems += nsems;
 
 	sma->sem_base = (struct sem *) &sma[1];
 
	for (i = 0; i < nsems; i++) {
 		INIT_LIST_HEAD(&sma->sem_base[i].sem_pending);
		spin_lock_init(&sma->sem_base[i].lock);
	}
 
 	sma->complex_count = 0;
 	INIT_LIST_HEAD(&sma->sem_pending);
 	INIT_LIST_HEAD(&sma->list_id);
 	sma->sem_nsems = nsems;
 	sma->sem_ctime = get_seconds();
	sem_unlock(sma, -1);
 
 	return sma->sem_perm.id;
 }
