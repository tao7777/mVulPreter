static ssize_t aio_setup_single_vector(struct kiocb *kiocb,
				       int rw, char __user *buf,
 				       unsigned long *nr_segs,
 				       struct iovec *iovec)
 {
	size_t len = kiocb->ki_nbytes;

	if (len > MAX_RW_COUNT)
		len = MAX_RW_COUNT;

	if (unlikely(!access_ok(!rw, buf, len)))
 		return -EFAULT;
 
 	iovec->iov_base = buf;
	iovec->iov_len = len;
 	*nr_segs = 1;
 	return 0;
 }
