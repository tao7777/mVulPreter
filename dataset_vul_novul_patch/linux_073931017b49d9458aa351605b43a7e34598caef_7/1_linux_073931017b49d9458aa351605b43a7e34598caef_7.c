int jffs2_set_acl(struct inode *inode, struct posix_acl *acl, int type)
{
	int rc, xprefix;

	switch (type) {
 	case ACL_TYPE_ACCESS:
 		xprefix = JFFS2_XPREFIX_ACL_ACCESS;
 		if (acl) {
			umode_t mode = inode->i_mode;
			rc = posix_acl_equiv_mode(acl, &mode);
			if (rc < 0)
 				return rc;
 			if (inode->i_mode != mode) {
 				struct iattr attr;

				attr.ia_valid = ATTR_MODE | ATTR_CTIME;
				attr.ia_mode = mode;
				attr.ia_ctime = CURRENT_TIME_SEC;
				rc = jffs2_do_setattr(inode, &attr);
 				if (rc < 0)
 					return rc;
 			}
			if (rc == 0)
				acl = NULL;
 		}
 		break;
 	case ACL_TYPE_DEFAULT:
		xprefix = JFFS2_XPREFIX_ACL_DEFAULT;
		if (!S_ISDIR(inode->i_mode))
			return acl ? -EACCES : 0;
		break;
	default:
		return -EINVAL;
	}
	rc = __jffs2_set_acl(inode, xprefix, acl);
	if (!rc)
		set_cached_acl(inode, type, acl);
	return rc;
}
