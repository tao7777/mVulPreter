static int _nfs4_do_open(struct inode *dir, struct path *path, int flags, struct iattr *sattr, struct rpc_cred *cred, struct nfs4_state **res)
static int _nfs4_do_open(struct inode *dir, struct path *path, fmode_t fmode, int flags, struct iattr *sattr, struct rpc_cred *cred, struct nfs4_state **res)
 {
 	struct nfs4_state_owner  *sp;
 	struct nfs4_state     *state = NULL;
	struct nfs_server       *server = NFS_SERVER(dir);
	struct nfs4_opendata *opendata;
	int status;

	/* Protect against reboot recovery conflicts */
	status = -ENOMEM;
	if (!(sp = nfs4_get_state_owner(server, cred))) {
		dprintk("nfs4_do_open: nfs4_get_state_owner failed!\n");
		goto out_err;
	}
	status = nfs4_recover_expired_lease(server);
 	if (status != 0)
 		goto err_put_state_owner;
 	if (path->dentry->d_inode != NULL)
		nfs4_return_incompatible_delegation(path->dentry->d_inode, fmode);
 	status = -ENOMEM;
	opendata = nfs4_opendata_alloc(path, sp, fmode, flags, sattr);
 	if (opendata == NULL)
 		goto err_put_state_owner;
 
	if (path->dentry->d_inode != NULL)
		opendata->state = nfs4_get_open_state(path->dentry->d_inode, sp);

	status = _nfs4_proc_open(opendata);
	if (status != 0)
		goto err_opendata_put;

	if (opendata->o_arg.open_flags & O_EXCL)
		nfs4_exclusive_attrset(opendata, sattr);

	state = nfs4_opendata_to_nfs4_state(opendata);
	status = PTR_ERR(state);
	if (IS_ERR(state))
		goto err_opendata_put;
	nfs4_opendata_put(opendata);
	nfs4_put_state_owner(sp);
	*res = state;
	return 0;
err_opendata_put:
	nfs4_opendata_put(opendata);
err_put_state_owner:
	nfs4_put_state_owner(sp);
out_err:
	*res = NULL;
	return status;
 }
