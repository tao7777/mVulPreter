 static inline void sem_putref(struct sem_array *sma)
 {
	sem_lock_and_putref(sma);
	sem_unlock(sma, -1);
 }
