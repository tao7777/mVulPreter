__ext4_set_acl(handle_t *handle, struct inode *inode, int type,
	     struct posix_acl *acl)
{
	int name_index;
	void *value = NULL;
	size_t size = 0;
	int error;

	switch (type) {
 	case ACL_TYPE_ACCESS:
 		name_index = EXT4_XATTR_INDEX_POSIX_ACL_ACCESS;
 		if (acl) {
			error = posix_acl_update_mode(inode, &inode->i_mode, &acl);
			if (error)
 				return error;
			inode->i_ctime = ext4_current_time(inode);
			ext4_mark_inode_dirty(handle, inode);
 		}
 		break;
 
	case ACL_TYPE_DEFAULT:
		name_index = EXT4_XATTR_INDEX_POSIX_ACL_DEFAULT;
		if (!S_ISDIR(inode->i_mode))
			return acl ? -EACCES : 0;
		break;

	default:
		return -EINVAL;
	}
	if (acl) {
		value = ext4_acl_to_disk(acl, &size);
		if (IS_ERR(value))
			return (int)PTR_ERR(value);
	}

	error = ext4_xattr_set_handle(handle, inode, name_index, "",
				      value, size, 0);

	kfree(value);
	if (!error)
		set_cached_acl(inode, type, acl);

	return error;
}
