SYSCALL_DEFINE6(sendto, int, fd, void __user *, buff, size_t, len,
		unsigned int, flags, struct sockaddr __user *, addr,
		int, addr_len)
{
	struct socket *sock;
	struct sockaddr_storage address;
	int err;
	struct msghdr msg;
	struct iovec iov;
	int fput_needed;
 
 	if (len > INT_MAX)
 		len = INT_MAX;
	if (unlikely(!access_ok(VERIFY_READ, buff, len)))
		return -EFAULT;
 	sock = sockfd_lookup_light(fd, &err, &fput_needed);
 	if (!sock)
 		goto out;

	iov.iov_base = buff;
	iov.iov_len = len;
	msg.msg_name = NULL;
	iov_iter_init(&msg.msg_iter, WRITE, &iov, 1, len);
	msg.msg_control = NULL;
	msg.msg_controllen = 0;
	msg.msg_namelen = 0;
	if (addr) {
		err = move_addr_to_kernel(addr, addr_len, &address);
		if (err < 0)
			goto out_put;
		msg.msg_name = (struct sockaddr *)&address;
		msg.msg_namelen = addr_len;
	}
	if (sock->file->f_flags & O_NONBLOCK)
		flags |= MSG_DONTWAIT;
	msg.msg_flags = flags;
	err = sock_sendmsg(sock, &msg, len);

out_put:
	fput_light(sock->file, fput_needed);
out:
	return err;
}
