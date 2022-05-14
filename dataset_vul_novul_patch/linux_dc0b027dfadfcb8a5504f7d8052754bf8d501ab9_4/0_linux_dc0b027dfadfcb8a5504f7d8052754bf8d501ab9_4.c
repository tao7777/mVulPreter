static int can_open_delegated(struct nfs_delegation *delegation, mode_t open_flags)
static int can_open_delegated(struct nfs_delegation *delegation, fmode_t fmode)
 {
	if ((delegation->type & fmode) != fmode)
 		return 0;
 	if (test_bit(NFS_DELEGATION_NEED_RECLAIM, &delegation->flags))
 		return 0;
 	nfs_mark_delegation_referenced(delegation);
 	return 1;
 }
