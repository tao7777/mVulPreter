static long vbg_misc_device_ioctl(struct file *filp, unsigned int req,
				  unsigned long arg)
{
	struct vbg_session *session = filp->private_data;
	size_t returned_size, size;
	struct vbg_ioctl_hdr hdr;
	bool is_vmmdev_req;
	int ret = 0;
	void *buf;

	if (copy_from_user(&hdr, (void *)arg, sizeof(hdr)))
		return -EFAULT;

	if (hdr.version != VBG_IOCTL_HDR_VERSION)
		return -EINVAL;

	if (hdr.size_in < sizeof(hdr) ||
	    (hdr.size_out && hdr.size_out < sizeof(hdr)))
		return -EINVAL;

	size = max(hdr.size_in, hdr.size_out);
	if (_IOC_SIZE(req) && _IOC_SIZE(req) != size)
		return -EINVAL;
	if (size > SZ_16M)
		return -E2BIG;

	/*
	 * IOCTL_VMMDEV_REQUEST needs the buffer to be below 4G to avoid
	 * the need for a bounce-buffer and another copy later on.
	 */
	is_vmmdev_req = (req & ~IOCSIZE_MASK) == VBG_IOCTL_VMMDEV_REQUEST(0) ||
			 req == VBG_IOCTL_VMMDEV_REQUEST_BIG;

	if (is_vmmdev_req)
		buf = vbg_req_alloc(size, VBG_IOCTL_HDR_TYPE_DEFAULT);
	else
		buf = kmalloc(size, GFP_KERNEL);
 	if (!buf)
 		return -ENOMEM;
 
	*((struct vbg_ioctl_hdr *)buf) = hdr;
	if (copy_from_user(buf + sizeof(hdr), (void *)arg + sizeof(hdr),
			   hdr.size_in - sizeof(hdr))) {
 		ret = -EFAULT;
 		goto out;
 	}
	if (hdr.size_in < size)
		memset(buf + hdr.size_in, 0, size -  hdr.size_in);

	ret = vbg_core_ioctl(session, req, buf);
	if (ret)
		goto out;

	returned_size = ((struct vbg_ioctl_hdr *)buf)->size_out;
	if (returned_size > size) {
		vbg_debug("%s: too much output data %zu > %zu\n",
			  __func__, returned_size, size);
		returned_size = size;
	}
	if (copy_to_user((void *)arg, buf, returned_size) != 0)
		ret = -EFAULT;

out:
	if (is_vmmdev_req)
		vbg_req_free(buf, size);
	else
		kfree(buf);

	return ret;
}
