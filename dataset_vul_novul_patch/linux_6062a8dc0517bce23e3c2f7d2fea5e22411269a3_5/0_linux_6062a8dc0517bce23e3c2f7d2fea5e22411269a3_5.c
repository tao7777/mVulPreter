 static inline void sem_getref(struct sem_array *sma)
 {
	sem_lock(sma, NULL, -1);
	WARN_ON_ONCE(!ipc_rcu_getref(sma));
	sem_unlock(sma, -1);
 }
