static int ___sys_recvmsg(struct socket *sock, struct msghdr __user *msg,
			 struct msghdr *msg_sys, unsigned int flags, int nosec)
{
	struct compat_msghdr __user *msg_compat =
	    (struct compat_msghdr __user *)msg;
	struct iovec iovstack[UIO_FASTIOV];
	struct iovec *iov = iovstack;
	unsigned long cmsg_ptr;
	int err, total_len, len;

	/* kernel mode address */
	struct sockaddr_storage addr;

	/* user mode address pointers */
	struct sockaddr __user *uaddr;
	int __user *uaddr_len;

	if (MSG_CMSG_COMPAT & flags) {
		if (get_compat_msghdr(msg_sys, msg_compat))
			return -EFAULT;
	} else {
		err = copy_msghdr_from_user(msg_sys, msg);
		if (err)
			return err;
	}

	if (msg_sys->msg_iovlen > UIO_FASTIOV) {
		err = -EMSGSIZE;
		if (msg_sys->msg_iovlen > UIO_MAXIOV)
			goto out;
		err = -ENOMEM;
		iov = kmalloc(msg_sys->msg_iovlen * sizeof(struct iovec),
			      GFP_KERNEL);
		if (!iov)
 			goto out;
 	}
 
	/* Save the user-mode address (verify_iovec will change the
	 * kernel msghdr to use the kernel address space)
 	 */
 	uaddr = (__force void __user *)msg_sys->msg_name;
 	uaddr_len = COMPAT_NAMELEN(msg);
	if (MSG_CMSG_COMPAT & flags)
 		err = verify_compat_iovec(msg_sys, iov, &addr, VERIFY_WRITE);
	else
 		err = verify_iovec(msg_sys, iov, &addr, VERIFY_WRITE);
 	if (err < 0)
 		goto out_freeiov;
	total_len = err;

 	cmsg_ptr = (unsigned long)msg_sys->msg_control;
 	msg_sys->msg_flags = flags & (MSG_CMSG_CLOEXEC|MSG_CMSG_COMPAT);
 
	/* We assume all kernel code knows the size of sockaddr_storage */
	msg_sys->msg_namelen = 0;

 	if (sock->file->f_flags & O_NONBLOCK)
 		flags |= MSG_DONTWAIT;
 	err = (nosec ? sock_recvmsg_nosec : sock_recvmsg)(sock, msg_sys,
							  total_len, flags);
	if (err < 0)
		goto out_freeiov;
	len = err;

	if (uaddr != NULL) {
		err = move_addr_to_user(&addr,
					msg_sys->msg_namelen, uaddr,
					uaddr_len);
		if (err < 0)
			goto out_freeiov;
	}
	err = __put_user((msg_sys->msg_flags & ~MSG_CMSG_COMPAT),
			 COMPAT_FLAGS(msg));
	if (err)
		goto out_freeiov;
	if (MSG_CMSG_COMPAT & flags)
		err = __put_user((unsigned long)msg_sys->msg_control - cmsg_ptr,
				 &msg_compat->msg_controllen);
	else
		err = __put_user((unsigned long)msg_sys->msg_control - cmsg_ptr,
				 &msg->msg_controllen);
	if (err)
		goto out_freeiov;
	err = len;

out_freeiov:
	if (iov != iovstack)
		kfree(iov);
out:
	return err;
}
