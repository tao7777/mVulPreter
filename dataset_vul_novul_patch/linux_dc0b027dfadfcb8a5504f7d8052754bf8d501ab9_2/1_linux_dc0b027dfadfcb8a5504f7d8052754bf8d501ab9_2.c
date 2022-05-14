static int _nfs4_do_open_reclaim(struct nfs_open_context *ctx, struct nfs4_state *state)
 {
 	struct nfs_delegation *delegation;
 	struct nfs4_opendata *opendata;
	int delegation_type = 0;
 	int status;
 
 	opendata = nfs4_open_recoverdata_alloc(ctx, state);
	if (IS_ERR(opendata))
		return PTR_ERR(opendata);
	opendata->o_arg.claim = NFS4_OPEN_CLAIM_PREVIOUS;
	opendata->o_arg.fh = NFS_FH(state->inode);
	rcu_read_lock();
	delegation = rcu_dereference(NFS_I(state->inode)->delegation);
	if (delegation != NULL && test_bit(NFS_DELEGATION_NEED_RECLAIM, &delegation->flags) != 0)
		delegation_type = delegation->type;
	rcu_read_unlock();
	opendata->o_arg.u.delegation_type = delegation_type;
	status = nfs4_open_recover(opendata, state);
	nfs4_opendata_put(opendata);
	return status;
}
