static long vop_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{
	struct vop_vdev *vdev = f->private_data;
	struct vop_info *vi = vdev->vi;
	void __user *argp = (void __user *)arg;
	int ret;

	switch (cmd) {
	case MIC_VIRTIO_ADD_DEVICE:
	{
		struct mic_device_desc dd, *dd_config;

		if (copy_from_user(&dd, argp, sizeof(dd)))
			return -EFAULT;

		if (mic_aligned_desc_size(&dd) > MIC_MAX_DESC_BLK_SIZE ||
		    dd.num_vq > MIC_MAX_VRINGS)
			return -EINVAL;

		dd_config = kzalloc(mic_desc_size(&dd), GFP_KERNEL);
		if (!dd_config)
			return -ENOMEM;
		if (copy_from_user(dd_config, argp, mic_desc_size(&dd))) {
 			ret = -EFAULT;
 			goto free_ret;
 		}
		/* Ensure desc has not changed between the two reads */
		if (memcmp(&dd, dd_config, sizeof(dd))) {
			ret = -EINVAL;
			goto free_ret;
		}
 		mutex_lock(&vdev->vdev_mutex);
 		mutex_lock(&vi->vop_mutex);
 		ret = vop_virtio_add_device(vdev, dd_config);
		if (ret)
			goto unlock_ret;
		list_add_tail(&vdev->list, &vi->vdev_list);
unlock_ret:
		mutex_unlock(&vi->vop_mutex);
		mutex_unlock(&vdev->vdev_mutex);
free_ret:
		kfree(dd_config);
		return ret;
	}
	case MIC_VIRTIO_COPY_DESC:
	{
		struct mic_copy_desc copy;

		mutex_lock(&vdev->vdev_mutex);
		ret = vop_vdev_inited(vdev);
		if (ret)
			goto _unlock_ret;

		if (copy_from_user(&copy, argp, sizeof(copy))) {
			ret = -EFAULT;
			goto _unlock_ret;
		}

		ret = vop_virtio_copy_desc(vdev, &copy);
		if (ret < 0)
			goto _unlock_ret;
		if (copy_to_user(
			&((struct mic_copy_desc __user *)argp)->out_len,
			&copy.out_len, sizeof(copy.out_len)))
			ret = -EFAULT;
_unlock_ret:
		mutex_unlock(&vdev->vdev_mutex);
		return ret;
	}
	case MIC_VIRTIO_CONFIG_CHANGE:
	{
		void *buf;

		mutex_lock(&vdev->vdev_mutex);
		ret = vop_vdev_inited(vdev);
		if (ret)
			goto __unlock_ret;
		buf = kzalloc(vdev->dd->config_len, GFP_KERNEL);
		if (!buf) {
			ret = -ENOMEM;
			goto __unlock_ret;
		}
		if (copy_from_user(buf, argp, vdev->dd->config_len)) {
			ret = -EFAULT;
			goto done;
		}
		ret = vop_virtio_config_change(vdev, buf);
done:
		kfree(buf);
__unlock_ret:
		mutex_unlock(&vdev->vdev_mutex);
		return ret;
	}
	default:
		return -ENOIOCTLCMD;
	};
	return 0;
}
