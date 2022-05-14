static void follow_dotdot(struct nameidata *nd)
 {
 	if (!nd->root.mnt)
 		set_root(nd);

	while(1) {
		struct dentry *old = nd->path.dentry;

		if (nd->path.dentry == nd->root.dentry &&
		    nd->path.mnt == nd->root.mnt) {
			break;
		}
		if (nd->path.dentry != nd->path.mnt->mnt_root) {
 			/* rare case of legitimate dget_parent()... */
 			nd->path.dentry = dget_parent(nd->path.dentry);
 			dput(old);
 			break;
 		}
 		if (!follow_up(&nd->path))
 			break;
 	}
 	follow_mount(&nd->path);
 	nd->inode = nd->path.dentry->d_inode;
 }
