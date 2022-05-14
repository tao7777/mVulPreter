 static inline void sem_lock_and_putref(struct sem_array *sma)
 {
	ipc_lock_by_ptr(&sma->sem_perm);
 	ipc_rcu_putref(sma);
 }
