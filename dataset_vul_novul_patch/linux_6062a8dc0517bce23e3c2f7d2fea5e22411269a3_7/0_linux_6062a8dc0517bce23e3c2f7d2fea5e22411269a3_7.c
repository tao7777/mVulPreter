 static inline void sem_lock_and_putref(struct sem_array *sma)
 {
	rcu_read_lock();
	sem_lock(sma, NULL, -1);
 	ipc_rcu_putref(sma);
 }
