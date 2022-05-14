 static inline void sem_putref(struct sem_array *sma)
 {
	ipc_lock_by_ptr(&sma->sem_perm);
	ipc_rcu_putref(sma);
	ipc_unlock(&(sma)->sem_perm);
 }
