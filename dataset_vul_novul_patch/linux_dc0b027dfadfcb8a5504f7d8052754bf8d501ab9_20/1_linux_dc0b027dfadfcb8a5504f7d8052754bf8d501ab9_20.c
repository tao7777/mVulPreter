static void nfs4_return_incompatible_delegation(struct inode *inode, mode_t open_flags)
 {
 	struct nfs_delegation *delegation;
 
 	rcu_read_lock();
 	delegation = rcu_dereference(NFS_I(inode)->delegation);
	if (delegation == NULL || (delegation->type & open_flags) == open_flags) {
 		rcu_read_unlock();
 		return;
 	}
	rcu_read_unlock();
	nfs_inode_return_delegation(inode);
}
