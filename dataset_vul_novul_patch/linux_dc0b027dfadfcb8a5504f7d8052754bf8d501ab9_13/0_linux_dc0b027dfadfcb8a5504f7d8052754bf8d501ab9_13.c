static int nfs4_open_recover_helper(struct nfs4_opendata *opendata, mode_t openflags, struct nfs4_state **res)
static int nfs4_open_recover_helper(struct nfs4_opendata *opendata, fmode_t fmode, struct nfs4_state **res)
 {
 	struct nfs4_state *newstate;
 	int ret;
 
	opendata->o_arg.open_flags = 0;
	opendata->o_arg.fmode = fmode;
 	memset(&opendata->o_res, 0, sizeof(opendata->o_res));
 	memset(&opendata->c_res, 0, sizeof(opendata->c_res));
 	nfs4_init_opendata_res(opendata);
	ret = _nfs4_proc_open(opendata);
	if (ret != 0)
		return ret; 
 	newstate = nfs4_opendata_to_nfs4_state(opendata);
 	if (IS_ERR(newstate))
 		return PTR_ERR(newstate);
	nfs4_close_state(&opendata->path, newstate, fmode);
 	*res = newstate;
 	return 0;
 }
