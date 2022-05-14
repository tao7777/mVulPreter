long kernel_wait4(pid_t upid, int __user *stat_addr, int options,
		  struct rusage *ru)
{
	struct wait_opts wo;
	struct pid *pid = NULL;
	enum pid_type type;
	long ret;

	if (options & ~(WNOHANG|WUNTRACED|WCONTINUED|
 			__WNOTHREAD|__WCLONE|__WALL))
 		return -EINVAL;
 
 	if (upid == -1)
 		type = PIDTYPE_MAX;
 	else if (upid < 0) {
		type = PIDTYPE_PGID;
		pid = find_get_pid(-upid);
	} else if (upid == 0) {
		type = PIDTYPE_PGID;
		pid = get_task_pid(current, PIDTYPE_PGID);
	} else /* upid > 0 */ {
		type = PIDTYPE_PID;
		pid = find_get_pid(upid);
	}

	wo.wo_type	= type;
	wo.wo_pid	= pid;
	wo.wo_flags	= options | WEXITED;
	wo.wo_info	= NULL;
	wo.wo_stat	= 0;
	wo.wo_rusage	= ru;
	ret = do_wait(&wo);
	put_pid(pid);
	if (ret > 0 && stat_addr && put_user(wo.wo_stat, stat_addr))
		ret = -EFAULT;

	return ret;
}
