static int cuse_channel_release(struct inode *inode, struct file *file)
{
	struct fuse_dev *fud = file->private_data;
	struct cuse_conn *cc = fc_to_cc(fud->fc);
	int rc;

	/* remove from the conntbl, no more access from this point on */
	mutex_lock(&cuse_lock);
	list_del_init(&cc->list);
	mutex_unlock(&cuse_lock);

	/* remove device */
	if (cc->dev)
		device_unregister(cc->dev);
	if (cc->cdev) {
 		unregister_chrdev_region(cc->cdev->dev, 1);
 		cdev_del(cc->cdev);
 	}
	/* Base reference is now owned by "fud" */
	fuse_conn_put(&cc->fc);
 
 	rc = fuse_dev_release(inode, file);	/* puts the base reference */
 
	return rc;
}
