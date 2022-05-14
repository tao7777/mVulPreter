static void nfs4_open_prepare(struct rpc_task *task, void *calldata)
{
	struct nfs4_opendata *data = calldata;
	struct nfs4_state_owner *sp = data->owner;

	if (nfs_wait_on_sequence(data->o_arg.seqid, task) != 0)
		return;
	/*
	 * Check if we still need to send an OPEN call, or if we can use
	 * a delegation instead.
	 */
 	if (data->state != NULL) {
 		struct nfs_delegation *delegation;
 
		if (can_open_cached(data->state, data->o_arg.fmode, data->o_arg.open_flags))
 			goto out_no_action;
 		rcu_read_lock();
 		delegation = rcu_dereference(NFS_I(data->state->inode)->delegation);
		if (delegation != NULL &&
		    test_bit(NFS_DELEGATION_NEED_RECLAIM, &delegation->flags) == 0) {
			rcu_read_unlock();
			goto out_no_action;
		}
		rcu_read_unlock();
	}
	/* Update sequence id. */
	data->o_arg.id = sp->so_owner_id.id;
	data->o_arg.clientid = sp->so_client->cl_clientid;
	if (data->o_arg.claim == NFS4_OPEN_CLAIM_PREVIOUS) {
		task->tk_msg.rpc_proc = &nfs4_procedures[NFSPROC4_CLNT_OPEN_NOATTR];
		nfs_copy_fh(&data->o_res.fh, data->o_arg.fh);
	}
	data->timestamp = jiffies;
	rpc_call_start(task);
	return;
out_no_action:
	task->tk_action = NULL;

}
