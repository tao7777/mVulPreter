xfs_set_acl(struct inode *inode, struct posix_acl *acl, int type)
{
	int error = 0;

	if (!acl)
		goto set_acl;

	error = -E2BIG;
	if (acl->a_count > XFS_ACL_MAX_ENTRIES(XFS_M(inode->i_sb)))
 		return error;
 
 	if (type == ACL_TYPE_ACCESS) {
		umode_t mode;
 
		error = posix_acl_update_mode(inode, &mode, &acl);
		if (error)
			return error;
 		error = xfs_set_mode(inode, mode);
 		if (error)
 			return error;
	}

 set_acl:
	return __xfs_set_acl(inode, type, acl);
}
