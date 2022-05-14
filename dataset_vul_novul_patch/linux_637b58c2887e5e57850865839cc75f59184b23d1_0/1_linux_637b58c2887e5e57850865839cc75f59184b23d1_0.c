pipe_iov_copy_to_user(struct iovec *iov, const void *from, unsigned long len,
		      int atomic)
{
	unsigned long copy;
	while (len > 0) {
		while (!iov->iov_len)
			iov++;
		copy = min_t(unsigned long, len, iov->iov_len);
		if (atomic) {
			if (__copy_to_user_inatomic(iov->iov_base, from, copy))
				return -EFAULT;
		} else {
			if (copy_to_user(iov->iov_base, from, copy))
				return -EFAULT;
		}
		from += copy;
		len -= copy;
		iov->iov_base += copy;
		iov->iov_len -= copy;
	}
	return 0;
}
