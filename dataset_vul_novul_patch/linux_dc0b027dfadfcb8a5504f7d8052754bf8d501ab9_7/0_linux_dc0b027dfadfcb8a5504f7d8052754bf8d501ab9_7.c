static void nfs4_close_prepare(struct rpc_task *task, void *data)
{
	struct nfs4_closedata *calldata = data;
	struct nfs4_state *state = calldata->state;
	int clear_rd, clear_wr, clear_rdwr;

	if (nfs_wait_on_sequence(calldata->arg.seqid, task) != 0)
		return;

	clear_rd = clear_wr = clear_rdwr = 0;
	spin_lock(&state->owner->so_lock);
	/* Calculate the change in open mode */
	if (state->n_rdwr == 0) {
		if (state->n_rdonly == 0) {
			clear_rd |= test_and_clear_bit(NFS_O_RDONLY_STATE, &state->flags);
			clear_rdwr |= test_and_clear_bit(NFS_O_RDWR_STATE, &state->flags);
		}
		if (state->n_wronly == 0) {
			clear_wr |= test_and_clear_bit(NFS_O_WRONLY_STATE, &state->flags);
			clear_rdwr |= test_and_clear_bit(NFS_O_RDWR_STATE, &state->flags);
		}
	}
	spin_unlock(&state->owner->so_lock);
	if (!clear_rd && !clear_wr && !clear_rdwr) {
		/* Note: exit _without_ calling nfs4_close_done */
		task->tk_action = NULL;
		return;
	}
 	nfs_fattr_init(calldata->res.fattr);
 	if (test_bit(NFS_O_RDONLY_STATE, &state->flags) != 0) {
 		task->tk_msg.rpc_proc = &nfs4_procedures[NFSPROC4_CLNT_OPEN_DOWNGRADE];
		calldata->arg.fmode = FMODE_READ;
 	} else if (test_bit(NFS_O_WRONLY_STATE, &state->flags) != 0) {
 		task->tk_msg.rpc_proc = &nfs4_procedures[NFSPROC4_CLNT_OPEN_DOWNGRADE];
		calldata->arg.fmode = FMODE_WRITE;
 	}
 	calldata->timestamp = jiffies;
 	rpc_call_start(task);
}
