bool inode_capable(const struct inode *inode, int cap)
bool capable_wrt_inode_uidgid(const struct inode *inode, int cap)
 {
 	struct user_namespace *ns = current_user_ns();
 
	return ns_capable(ns, cap) && kuid_has_mapping(ns, inode->i_uid) &&
		kgid_has_mapping(ns, inode->i_gid);
 }
