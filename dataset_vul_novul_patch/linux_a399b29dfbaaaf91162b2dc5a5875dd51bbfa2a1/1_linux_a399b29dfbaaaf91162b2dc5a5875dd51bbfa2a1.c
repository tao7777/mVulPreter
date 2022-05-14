SYSCALL_DEFINE3(shmctl, int, shmid, int, cmd, struct shmid_ds __user *, buf)
{
	struct shmid_kernel *shp;
	int err, version;
	struct ipc_namespace *ns;

	if (cmd < 0 || shmid < 0)
		return -EINVAL;

	version = ipc_parse_version(&cmd);
	ns = current->nsproxy->ipc_ns;

	switch (cmd) {
	case IPC_INFO:
	case SHM_INFO:
	case SHM_STAT:
	case IPC_STAT:
		return shmctl_nolock(ns, shmid, cmd, version, buf);
	case IPC_RMID:
	case IPC_SET:
		return shmctl_down(ns, shmid, cmd, buf, version);
	case SHM_LOCK:
	case SHM_UNLOCK:
	{
		struct file *shm_file;

		rcu_read_lock();
		shp = shm_obtain_object_check(ns, shmid);
		if (IS_ERR(shp)) {
			err = PTR_ERR(shp);
			goto out_unlock1;
		}

		audit_ipc_obj(&(shp->shm_perm));
		err = security_shm_shmctl(shp, cmd);
		if (err)
			goto out_unlock1;

		ipc_lock_object(&shp->shm_perm);
		if (!ns_capable(ns->user_ns, CAP_IPC_LOCK)) {
			kuid_t euid = current_euid();
			err = -EPERM;
			if (!uid_eq(euid, shp->shm_perm.uid) &&
			    !uid_eq(euid, shp->shm_perm.cuid))
				goto out_unlock0;
			if (cmd == SHM_LOCK && !rlimit(RLIMIT_MEMLOCK))
				goto out_unlock0;
 		}
 
 		shm_file = shp->shm_file;
 		if (is_file_hugepages(shm_file))
 			goto out_unlock0;
 
		if (cmd == SHM_LOCK) {
			struct user_struct *user = current_user();
			err = shmem_lock(shm_file, 1, user);
			if (!err && !(shp->shm_perm.mode & SHM_LOCKED)) {
				shp->shm_perm.mode |= SHM_LOCKED;
				shp->mlock_user = user;
			}
			goto out_unlock0;
		}

		/* SHM_UNLOCK */
		if (!(shp->shm_perm.mode & SHM_LOCKED))
			goto out_unlock0;
		shmem_lock(shm_file, 0, shp->mlock_user);
		shp->shm_perm.mode &= ~SHM_LOCKED;
		shp->mlock_user = NULL;
		get_file(shm_file);
		ipc_unlock_object(&shp->shm_perm);
		rcu_read_unlock();
		shmem_unlock_mapping(shm_file->f_mapping);

		fput(shm_file);
		return err;
	}
	default:
		return -EINVAL;
	}

out_unlock0:
	ipc_unlock_object(&shp->shm_perm);
out_unlock1:
	rcu_read_unlock();
	return err;
}
