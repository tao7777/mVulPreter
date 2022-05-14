static ssize_t nfs4_proc_get_acl(struct inode *inode, void *buf, size_t buflen)
{
	struct nfs_server *server = NFS_SERVER(inode);
	int ret;

	if (!nfs4_server_supports_acls(server))
		return -EOPNOTSUPP;
	ret = nfs_revalidate_inode(server, inode);
	if (ret < 0)
		return ret;
	if (NFS_I(inode)->cache_validity & NFS_INO_INVALID_ACL)
 		nfs_zap_acl_cache(inode);
 	ret = nfs4_read_cached_acl(inode, buf, buflen);
 	if (ret != -ENOENT)
		/* -ENOENT is returned if there is no ACL or if there is an ACL
		 * but no cached acl data, just the acl length */
 		return ret;
 	return nfs4_get_acl_uncached(inode, buf, buflen);
 }
