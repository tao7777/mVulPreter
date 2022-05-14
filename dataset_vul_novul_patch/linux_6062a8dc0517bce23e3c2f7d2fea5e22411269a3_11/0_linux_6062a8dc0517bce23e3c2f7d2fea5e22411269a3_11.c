static int semctl_down(struct ipc_namespace *ns, int semid,
		       int cmd, int version, void __user *p)
{
	struct sem_array *sma;
	int err;
	struct semid64_ds semid64;
	struct kern_ipc_perm *ipcp;

	if(cmd == IPC_SET) {
		if (copy_semid_from_user(&semid64, p, version))
			return -EFAULT;
	}

	ipcp = ipcctl_pre_down_nolock(ns, &sem_ids(ns), semid, cmd,
				      &semid64.sem_perm, 0);
	if (IS_ERR(ipcp))
		return PTR_ERR(ipcp);

	sma = container_of(ipcp, struct sem_array, sem_perm);

	err = security_sem_semctl(sma, cmd);
	if (err) {
		rcu_read_unlock();
		goto out_unlock;
	}
 
 	switch(cmd){
 	case IPC_RMID:
		sem_lock(sma, NULL, -1);
 		freeary(ns, ipcp);
 		goto out_up;
 	case IPC_SET:
		sem_lock(sma, NULL, -1);
 		err = ipc_update_perm(&semid64.sem_perm, ipcp);
 		if (err)
 			goto out_unlock;
		sma->sem_ctime = get_seconds();
		break;
	default:
		rcu_read_unlock();
		err = -EINVAL;
		goto out_up;
 	}
 
 out_unlock:
	sem_unlock(sma, -1);
 out_up:
 	up_write(&sem_ids(ns).rw_mutex);
 	return err;
}
