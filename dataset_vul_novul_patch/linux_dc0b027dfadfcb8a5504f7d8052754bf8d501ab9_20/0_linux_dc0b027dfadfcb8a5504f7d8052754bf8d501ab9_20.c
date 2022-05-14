static void nfs4_return_incompatible_delegation(struct inode *inode, mode_t open_flags)
static void nfs4_return_incompatible_delegation(struct inode *inode, fmode_t fmode)
 {
 	struct nfs_delegation *delegation;
 
 	rcu_read_lock();
 	delegation = rcu_dereference(NFS_I(inode)->delegation);
	if (delegation == NULL || (delegation->type & fmode) == fmode) {
 		rcu_read_unlock();
 		return;
 	}
	rcu_read_unlock();
	nfs_inode_return_delegation(inode);
}
