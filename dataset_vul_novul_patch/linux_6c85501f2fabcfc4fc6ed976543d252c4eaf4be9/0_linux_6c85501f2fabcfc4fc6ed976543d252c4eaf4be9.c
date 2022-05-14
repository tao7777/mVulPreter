COMPAT_SYSCALL_DEFINE5(waitid,
		int, which, compat_pid_t, pid,
		struct compat_siginfo __user *, infop, int, options,
		struct compat_rusage __user *, uru)
{
	struct rusage ru;
	struct waitid_info info = {.status = 0};
	long err = kernel_waitid(which, pid, &info, options, uru ? &ru : NULL);
	int signo = 0;
 	if (err > 0) {
 		signo = SIGCHLD;
 		err = 0;
		if (uru) {
			/* kernel_waitid() overwrites everything in ru */
			if (COMPAT_USE_64BIT_TIME)
				err = copy_to_user(uru, &ru, sizeof(ru));
			else
				err = put_compat_rusage(&ru, uru);
			if (err)
				return -EFAULT;
		}
 	}
 
 	if (!infop)
		return err;

	user_access_begin();
	unsafe_put_user(signo, &infop->si_signo, Efault);
	unsafe_put_user(0, &infop->si_errno, Efault);
	unsafe_put_user(info.cause, &infop->si_code, Efault);
	unsafe_put_user(info.pid, &infop->si_pid, Efault);
	unsafe_put_user(info.uid, &infop->si_uid, Efault);
	unsafe_put_user(info.status, &infop->si_status, Efault);
	user_access_end();
	return err;
Efault:
	user_access_end();
	return -EFAULT;
}
