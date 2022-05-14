int hfsplus_set_posix_acl(struct inode *inode, struct posix_acl *acl,
		int type)
{
	int err;
	char *xattr_name;
	size_t size = 0;
	char *value = NULL;

	hfs_dbg(ACL_MOD, "[%s]: ino %lu\n", __func__, inode->i_ino);

	switch (type) {
 	case ACL_TYPE_ACCESS:
 		xattr_name = XATTR_NAME_POSIX_ACL_ACCESS;
 		if (acl) {
			err = posix_acl_equiv_mode(acl, &inode->i_mode);
			if (err < 0)
 				return err;
 		}
 		err = 0;
		break;

	case ACL_TYPE_DEFAULT:
		xattr_name = XATTR_NAME_POSIX_ACL_DEFAULT;
		if (!S_ISDIR(inode->i_mode))
			return acl ? -EACCES : 0;
		break;

	default:
		return -EINVAL;
	}

	if (acl) {
		size = posix_acl_xattr_size(acl->a_count);
		if (unlikely(size > HFSPLUS_MAX_INLINE_DATA_SIZE))
			return -ENOMEM;
		value = (char *)hfsplus_alloc_attr_entry();
		if (unlikely(!value))
			return -ENOMEM;
		err = posix_acl_to_xattr(&init_user_ns, acl, value, size);
		if (unlikely(err < 0))
			goto end_set_acl;
	}

	err = __hfsplus_setxattr(inode, xattr_name, value, size, 0);

end_set_acl:
	hfsplus_destroy_attr_entry((hfsplus_attr_entry *)value);

	if (!err)
		set_cached_acl(inode, type, acl);

	return err;
}
