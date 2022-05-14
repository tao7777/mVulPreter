SYSCALL_DEFINE4(osf_wait4, pid_t, pid, int __user *, ustatus, int, options,
		struct rusage32 __user *, ur)
 {
 	struct rusage r;
 	long ret, err;
	unsigned int status = 0;
 	mm_segment_t old_fs;
 
 	if (!ur)
		return sys_wait4(pid, ustatus, options, NULL);

 	old_fs = get_fs();
 		
 	set_fs (KERNEL_DS);
	ret = sys_wait4(pid, (unsigned int __user *) &status, options,
			(struct rusage __user *) &r);
 	set_fs (old_fs);
 
 	if (!access_ok(VERIFY_WRITE, ur, sizeof(*ur)))
 		return -EFAULT;
 
 	err = 0;
	err |= put_user(status, ustatus);
 	err |= __put_user(r.ru_utime.tv_sec, &ur->ru_utime.tv_sec);
 	err |= __put_user(r.ru_utime.tv_usec, &ur->ru_utime.tv_usec);
 	err |= __put_user(r.ru_stime.tv_sec, &ur->ru_stime.tv_sec);
	err |= __put_user(r.ru_stime.tv_usec, &ur->ru_stime.tv_usec);
	err |= __put_user(r.ru_maxrss, &ur->ru_maxrss);
	err |= __put_user(r.ru_ixrss, &ur->ru_ixrss);
	err |= __put_user(r.ru_idrss, &ur->ru_idrss);
	err |= __put_user(r.ru_isrss, &ur->ru_isrss);
	err |= __put_user(r.ru_minflt, &ur->ru_minflt);
	err |= __put_user(r.ru_majflt, &ur->ru_majflt);
	err |= __put_user(r.ru_nswap, &ur->ru_nswap);
	err |= __put_user(r.ru_inblock, &ur->ru_inblock);
	err |= __put_user(r.ru_oublock, &ur->ru_oublock);
	err |= __put_user(r.ru_msgsnd, &ur->ru_msgsnd);
	err |= __put_user(r.ru_msgrcv, &ur->ru_msgrcv);
	err |= __put_user(r.ru_nsignals, &ur->ru_nsignals);
	err |= __put_user(r.ru_nvcsw, &ur->ru_nvcsw);
	err |= __put_user(r.ru_nivcsw, &ur->ru_nivcsw);

	return err ? err : ret;
}
