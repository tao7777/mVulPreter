__reiserfs_set_acl(struct reiserfs_transaction_handle *th, struct inode *inode,
		 int type, struct posix_acl *acl)
{
	char *name;
	void *value = NULL;
	size_t size = 0;
	int error;

	switch (type) {
 	case ACL_TYPE_ACCESS:
 		name = XATTR_NAME_POSIX_ACL_ACCESS;
 		if (acl) {
			error = posix_acl_equiv_mode(acl, &inode->i_mode);
			if (error < 0)
 				return error;
			else {
				if (error == 0)
					acl = NULL;
			}
 		}
 		break;
 	case ACL_TYPE_DEFAULT:
		name = XATTR_NAME_POSIX_ACL_DEFAULT;
		if (!S_ISDIR(inode->i_mode))
			return acl ? -EACCES : 0;
		break;
	default:
		return -EINVAL;
	}

	if (acl) {
		value = reiserfs_posix_acl_to_disk(acl, &size);
		if (IS_ERR(value))
			return (int)PTR_ERR(value);
	}

	error = reiserfs_xattr_set_handle(th, inode, name, value, size, 0);

	/*
	 * Ensure that the inode gets dirtied if we're only using
	 * the mode bits and an old ACL didn't exist. We don't need
	 * to check if the inode is hashed here since we won't get
	 * called by reiserfs_inherit_default_acl().
	 */
	if (error == -ENODATA) {
		error = 0;
		if (type == ACL_TYPE_ACCESS) {
			inode->i_ctime = CURRENT_TIME_SEC;
			mark_inode_dirty(inode);
		}
	}

	kfree(value);

	if (!error)
		set_cached_acl(inode, type, acl);

	return error;
}
