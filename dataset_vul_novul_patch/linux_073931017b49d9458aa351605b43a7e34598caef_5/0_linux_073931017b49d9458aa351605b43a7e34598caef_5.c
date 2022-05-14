int __gfs2_set_acl(struct inode *inode, struct posix_acl *acl, int type)
{
	int error;
	int len;
	char *data;
	const char *name = gfs2_acl_name(type);

	if (acl && acl->a_count > GFS2_ACL_MAX_ENTRIES(GFS2_SB(inode)))
		return -E2BIG;

 	if (type == ACL_TYPE_ACCESS) {
 		umode_t mode = inode->i_mode;
 
		error = posix_acl_update_mode(inode, &inode->i_mode, &acl);
		if (error)
 			return error;
		if (mode != inode->i_mode)
 			mark_inode_dirty(inode);
 	}
 
 	if (acl) {
		len = posix_acl_to_xattr(&init_user_ns, acl, NULL, 0);
		if (len == 0)
			return 0;
		data = kmalloc(len, GFP_NOFS);
		if (data == NULL)
			return -ENOMEM;
		error = posix_acl_to_xattr(&init_user_ns, acl, data, len);
		if (error < 0)
			goto out;
	} else {
		data = NULL;
		len = 0;
	}

	error = __gfs2_xattr_set(inode, name, data, len, 0, GFS2_EATYPE_SYS);
	if (error)
		goto out;
	set_cached_acl(inode, type, acl);
out:
	kfree(data);
	return error;
}
