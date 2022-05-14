static int __jfs_set_acl(tid_t tid, struct inode *inode, int type,
		       struct posix_acl *acl)
{
	char *ea_name;
	int rc;
	int size = 0;
	char *value = NULL;

	switch (type) {
 	case ACL_TYPE_ACCESS:
 		ea_name = XATTR_NAME_POSIX_ACL_ACCESS;
 		if (acl) {
			rc = posix_acl_update_mode(inode, &inode->i_mode, &acl);
			if (rc)
 				return rc;
 			inode->i_ctime = CURRENT_TIME;
 			mark_inode_dirty(inode);
 		}
 		break;
 	case ACL_TYPE_DEFAULT:
		ea_name = XATTR_NAME_POSIX_ACL_DEFAULT;
		break;
	default:
		return -EINVAL;
	}

	if (acl) {
		size = posix_acl_xattr_size(acl->a_count);
		value = kmalloc(size, GFP_KERNEL);
		if (!value)
			return -ENOMEM;
		rc = posix_acl_to_xattr(&init_user_ns, acl, value, size);
		if (rc < 0)
			goto out;
	}
	rc = __jfs_setxattr(tid, inode, ea_name, value, size, 0);
out:
	kfree(value);

	if (!rc)
		set_cached_acl(inode, type, acl);

	return rc;
}
