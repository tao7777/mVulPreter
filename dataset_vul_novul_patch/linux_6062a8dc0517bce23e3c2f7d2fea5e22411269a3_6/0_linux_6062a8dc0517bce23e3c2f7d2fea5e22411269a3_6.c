 static inline void sem_getref_and_unlock(struct sem_array *sma)
 {
	WARN_ON_ONCE(!ipc_rcu_getref(sma));
	sem_unlock(sma, -1);
 }
