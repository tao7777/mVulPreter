int verify_compat_iovec(struct msghdr *kern_msg, struct iovec *kern_iov,
		   struct sockaddr_storage *kern_address, int mode)
{
	int tot_len;

	if (kern_msg->msg_namelen) {
		if (mode == VERIFY_READ) {
			int err = move_addr_to_kernel(kern_msg->msg_name,
						      kern_msg->msg_namelen,
						      kern_address);
 			if (err < 0)
 				return err;
 		}
		if (kern_msg->msg_name)
			kern_msg->msg_name = kern_address;
 	} else
 		kern_msg->msg_name = NULL;
 
	tot_len = iov_from_user_compat_to_kern(kern_iov,
					  (struct compat_iovec __user *)kern_msg->msg_iov,
					  kern_msg->msg_iovlen);
	if (tot_len >= 0)
		kern_msg->msg_iov = kern_iov;

	return tot_len;
}
