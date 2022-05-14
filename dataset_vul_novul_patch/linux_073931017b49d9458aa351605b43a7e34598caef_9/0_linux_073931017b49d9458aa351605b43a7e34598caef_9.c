int ocfs2_set_acl(handle_t *handle,
			 struct inode *inode,
			 struct buffer_head *di_bh,
			 int type,
			 struct posix_acl *acl,
			 struct ocfs2_alloc_context *meta_ac,
			 struct ocfs2_alloc_context *data_ac)
{
	int name_index;
	void *value = NULL;
	size_t size = 0;
	int ret;

	if (S_ISLNK(inode->i_mode))
		return -EOPNOTSUPP;

	switch (type) {
 	case ACL_TYPE_ACCESS:
 		name_index = OCFS2_XATTR_INDEX_POSIX_ACL_ACCESS;
 		if (acl) {
			umode_t mode;
 
			ret = posix_acl_update_mode(inode, &mode, &acl);
			if (ret)
				return ret;
 
 			ret = ocfs2_acl_set_mode(inode, di_bh,
 						 handle, mode);
			if (ret)
				return ret;
		}
		break;
	case ACL_TYPE_DEFAULT:
		name_index = OCFS2_XATTR_INDEX_POSIX_ACL_DEFAULT;
		if (!S_ISDIR(inode->i_mode))
			return acl ? -EACCES : 0;
		break;
	default:
		return -EINVAL;
	}

	if (acl) {
		value = ocfs2_acl_to_xattr(acl, &size);
		if (IS_ERR(value))
			return (int)PTR_ERR(value);
	}

	if (handle)
		ret = ocfs2_xattr_set_handle(handle, inode, di_bh, name_index,
					     "", value, size, 0,
					     meta_ac, data_ac);
	else
		ret = ocfs2_xattr_set(inode, name_index, "", value, size, 0);

	kfree(value);

	return ret;
}
