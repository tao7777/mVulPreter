static int can_open_delegated(struct nfs_delegation *delegation, mode_t open_flags)
 {
	if ((delegation->type & open_flags) != open_flags)
 		return 0;
 	if (test_bit(NFS_DELEGATION_NEED_RECLAIM, &delegation->flags))
 		return 0;
 	nfs_mark_delegation_referenced(delegation);
 	return 1;
 }
