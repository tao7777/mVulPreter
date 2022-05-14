static inline struct sem_array *sem_lock_check(struct ipc_namespace *ns,
						int id)
{
	struct kern_ipc_perm *ipcp = ipc_lock_check(&sem_ids(ns), id);
	if (IS_ERR(ipcp))
		return ERR_CAST(ipcp);
	return container_of(ipcp, struct sem_array, sem_perm);
}
