static void nfs4_open_release(void *calldata)
{
	struct nfs4_opendata *data = calldata;
	struct nfs4_state *state = NULL;

	/* If this request hasn't been cancelled, do nothing */
	if (data->cancelled == 0)
		goto out_free;
	/* In case of error, no cleanup! */
	if (data->rpc_status != 0 || !data->rpc_done)
		goto out_free;
	/* In case we need an open_confirm, no cleanup! */
	if (data->o_res.rflags & NFS4_OPEN_RESULT_CONFIRM)
 		goto out_free;
 	state = nfs4_opendata_to_nfs4_state(data);
 	if (!IS_ERR(state))
		nfs4_close_state(&data->path, state, data->o_arg.fmode);
 out_free:
 	nfs4_opendata_put(data);
 }
