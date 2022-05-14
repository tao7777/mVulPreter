SYSCALL_DEFINE3(rt_sigqueueinfo, pid_t, pid, int, sig,
		siginfo_t __user *, uinfo)
{
	siginfo_t info;

	if (copy_from_user(&info, uinfo, sizeof(siginfo_t)))
 		return -EFAULT;
 
 	/* Not even root can pretend to send signals from the kernel.
	   Nor can they impersonate a kill(), which adds source info.  */
	if (info.si_code >= 0)
 		return -EPERM;
 	info.si_signo = sig;
 
 	/* POSIX.1b doesn't mention process groups.  */
	return kill_proc_info(sig, &info, pid);
}
