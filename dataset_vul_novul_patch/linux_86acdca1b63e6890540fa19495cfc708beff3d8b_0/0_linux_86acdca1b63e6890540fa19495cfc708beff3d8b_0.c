static void *proc_pid_follow_link(struct dentry *dentry, struct nameidata *nd)
{
	struct inode *inode = dentry->d_inode;
	int error = -EACCES;

	/* We don't need a base pointer in the /proc filesystem */
	path_put(&nd->path);

	/* Are we allowed to snoop on the tasks file descriptors? */
	if (!proc_fd_access_allowed(inode))
 		goto out;
 
 	error = PROC_I(inode)->op.proc_get_link(inode, &nd->path);
 out:
 	return ERR_PTR(error);
 }
