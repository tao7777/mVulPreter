asmlinkage long sys_oabi_fcntl64(unsigned int fd, unsigned int cmd,
static long do_locks(unsigned int fd, unsigned int cmd,
 				 unsigned long arg)
 {
 	struct flock64 kernel;
	struct oabi_flock64 user;
	mm_segment_t fs;
	long ret;

	if (copy_from_user(&user, (struct oabi_flock64 __user *)arg,
			   sizeof(user)))
		return -EFAULT;
	kernel.l_type	= user.l_type;
	kernel.l_whence	= user.l_whence;
	kernel.l_start	= user.l_start;
	kernel.l_len	= user.l_len;
	kernel.l_pid	= user.l_pid;

	fs = get_fs();
	set_fs(KERNEL_DS);
	ret = sys_fcntl64(fd, cmd, (unsigned long)&kernel);
	set_fs(fs);

	if (!ret && (cmd == F_GETLK64 || cmd == F_OFD_GETLK)) {
		user.l_type	= kernel.l_type;
		user.l_whence	= kernel.l_whence;
		user.l_start	= kernel.l_start;
		user.l_len	= kernel.l_len;
		user.l_pid	= kernel.l_pid;
		if (copy_to_user((struct oabi_flock64 __user *)arg,
				 &user, sizeof(user)))
			ret = -EFAULT;
	}
	return ret;
}
 
asmlinkage long sys_oabi_fcntl64(unsigned int fd, unsigned int cmd,
				 unsigned long arg)
{
 	switch (cmd) {
 	case F_OFD_GETLK:
 	case F_OFD_SETLK:
 	case F_OFD_SETLKW:
 	case F_GETLK64:
 	case F_SETLK64:
 	case F_SETLKW64:
		return do_locks(fd, cmd, arg);
 
	default:
		return sys_fcntl64(fd, cmd, arg);
 	}
 }
