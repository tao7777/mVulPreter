static int __btrfs_set_acl(struct btrfs_trans_handle *trans,
			 struct inode *inode, struct posix_acl *acl, int type)
{
	int ret, size = 0;
	const char *name;
	char *value = NULL;

	switch (type) {
 	case ACL_TYPE_ACCESS:
 		name = XATTR_NAME_POSIX_ACL_ACCESS;
 		if (acl) {
			ret = posix_acl_equiv_mode(acl, &inode->i_mode);
			if (ret < 0)
 				return ret;
			if (ret == 0)
				acl = NULL;
 		}
 		ret = 0;
 		break;
	case ACL_TYPE_DEFAULT:
		if (!S_ISDIR(inode->i_mode))
			return acl ? -EINVAL : 0;
		name = XATTR_NAME_POSIX_ACL_DEFAULT;
		break;
	default:
		return -EINVAL;
	}

	if (acl) {
		size = posix_acl_xattr_size(acl->a_count);
		value = kmalloc(size, GFP_KERNEL);
		if (!value) {
			ret = -ENOMEM;
			goto out;
		}

		ret = posix_acl_to_xattr(&init_user_ns, acl, value, size);
		if (ret < 0)
			goto out;
	}

	ret = __btrfs_setxattr(trans, inode, name, value, size, 0);
out:
	kfree(value);

	if (!ret)
		set_cached_acl(inode, type, acl);

	return ret;
}
