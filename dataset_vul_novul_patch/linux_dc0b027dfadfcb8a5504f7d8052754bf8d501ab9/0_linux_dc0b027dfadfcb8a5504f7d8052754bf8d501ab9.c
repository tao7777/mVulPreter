struct nfs_open_context *nfs_find_open_context(struct inode *inode, struct rpc_cred *cred, int mode)
struct nfs_open_context *nfs_find_open_context(struct inode *inode, struct rpc_cred *cred, fmode_t mode)
 {
 	struct nfs_inode *nfsi = NFS_I(inode);
 	struct nfs_open_context *pos, *ctx = NULL;

	spin_lock(&inode->i_lock);
	list_for_each_entry(pos, &nfsi->open_files, list) {
		if (cred != NULL && pos->cred != cred)
			continue;
		if ((pos->mode & mode) == mode) {
			ctx = get_nfs_open_context(pos);
			break;
		}
	}
	spin_unlock(&inode->i_lock);
	return ctx;
}
