static int iov_fault_in_pages_write(struct iovec *iov, unsigned long len)
{
	while (!iov->iov_len)
		iov++;
	while (len > 0) {
		unsigned long this_len;
		this_len = min_t(unsigned long, len, iov->iov_len);
		if (fault_in_pages_writeable(iov->iov_base, this_len))
			break;
		len -= this_len;
		iov++;
	}
	return len;
}
