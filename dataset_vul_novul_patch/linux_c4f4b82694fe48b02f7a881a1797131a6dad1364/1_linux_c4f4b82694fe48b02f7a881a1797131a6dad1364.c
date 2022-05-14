static ssize_t aio_setup_single_vector(struct kiocb *kiocb,
				       int rw, char __user *buf,
 				       unsigned long *nr_segs,
 				       struct iovec *iovec)
 {
	if (unlikely(!access_ok(!rw, buf, kiocb->ki_nbytes)))
 		return -EFAULT;
 
 	iovec->iov_base = buf;
	iovec->iov_len = kiocb->ki_nbytes;
 	*nr_segs = 1;
 	return 0;
 }
