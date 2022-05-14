pipe_iov_copy_from_user(void *to, struct iovec *iov, unsigned long len,
			int atomic)
{
	unsigned long copy;
	while (len > 0) {
		while (!iov->iov_len)
			iov++;
		copy = min_t(unsigned long, len, iov->iov_len);
		if (atomic) {
			if (__copy_from_user_inatomic(to, iov->iov_base, copy))
				return -EFAULT;
		} else {
			if (copy_from_user(to, iov->iov_base, copy))
				return -EFAULT;
		}
		to += copy;
		len -= copy;
		iov->iov_base += copy;
		iov->iov_len -= copy;
	}
	return 0;
}
