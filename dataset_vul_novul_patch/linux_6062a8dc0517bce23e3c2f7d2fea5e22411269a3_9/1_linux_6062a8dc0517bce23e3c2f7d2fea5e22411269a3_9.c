static inline struct sem_array *sem_obtain_lock(struct ipc_namespace *ns, int id)
 {
 	struct kern_ipc_perm *ipcp;
 	struct sem_array *sma;

	rcu_read_lock();
	ipcp = ipc_obtain_object(&sem_ids(ns), id);
	if (IS_ERR(ipcp)) {
		sma = ERR_CAST(ipcp);
 		goto err;
 	}
 
	spin_lock(&ipcp->lock);
 
 	/* ipc_rmid() may have already freed the ID while sem_lock
 	 * was spinning: verify that the structure is still valid
 	 */
 	if (!ipcp->deleted)
 		return container_of(ipcp, struct sem_array, sem_perm);
 
	spin_unlock(&ipcp->lock);
 	sma = ERR_PTR(-EINVAL);
 err:
 	rcu_read_unlock();
	return sma;
}
