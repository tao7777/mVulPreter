static void iov_fault_in_pages_read(struct iovec *iov, unsigned long len)
{
	while (!iov->iov_len)
		iov++;
	while (len > 0) {
		unsigned long this_len;
		this_len = min_t(unsigned long, len, iov->iov_len);
		fault_in_pages_readable(iov->iov_base, this_len);
		len -= this_len;
		iov++;
	}
}
