static int __f2fs_set_acl(struct inode *inode, int type,
			struct posix_acl *acl, struct page *ipage)
{
	int name_index;
	void *value = NULL;
	size_t size = 0;
	int error;

	switch (type) {
 	case ACL_TYPE_ACCESS:
 		name_index = F2FS_XATTR_INDEX_POSIX_ACL_ACCESS;
 		if (acl) {
			error = posix_acl_equiv_mode(acl, &inode->i_mode);
			if (error < 0)
 				return error;
 			set_acl_inode(inode, inode->i_mode);
			if (error == 0)
				acl = NULL;
 		}
 		break;
 
	case ACL_TYPE_DEFAULT:
		name_index = F2FS_XATTR_INDEX_POSIX_ACL_DEFAULT;
		if (!S_ISDIR(inode->i_mode))
			return acl ? -EACCES : 0;
		break;

	default:
		return -EINVAL;
	}

	if (acl) {
		value = f2fs_acl_to_disk(acl, &size);
		if (IS_ERR(value)) {
			clear_inode_flag(inode, FI_ACL_MODE);
			return (int)PTR_ERR(value);
		}
	}

	error = f2fs_setxattr(inode, name_index, "", value, size, ipage, 0);

	kfree(value);
	if (!error)
		set_cached_acl(inode, type, acl);

	clear_inode_flag(inode, FI_ACL_MODE);
	return error;
}
