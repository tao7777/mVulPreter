nfs4_open_revalidate(struct inode *dir, struct dentry *dentry, int openflags, struct nameidata *nd)
{
	struct path path = {
		.mnt = nd->path.mnt,
		.dentry = dentry,
 	};
 	struct rpc_cred *cred;
 	struct nfs4_state *state;
	fmode_t fmode = openflags & (FMODE_READ | FMODE_WRITE);
 
 	cred = rpc_lookup_cred();
 	if (IS_ERR(cred))
 		return PTR_ERR(cred);
	state = nfs4_do_open(dir, &path, fmode, openflags, NULL, cred);
 	put_rpccred(cred);
 	if (IS_ERR(state)) {
 		switch (PTR_ERR(state)) {
			case -EPERM:
			case -EACCES:
			case -EDQUOT:
			case -ENOSPC:
			case -EROFS:
				lookup_instantiate_filp(nd, (struct dentry *)state, NULL);
				return 1;
			default:
				goto out_drop;
		}
 	}
 	if (state->inode == dentry->d_inode) {
 		nfs_set_verifier(dentry, nfs_save_change_attribute(dir));
		nfs4_intent_set_file(nd, &path, state, fmode);
 		return 1;
 	}
	nfs4_close_sync(&path, state, fmode);
 out_drop:
 	d_drop(dentry);
 	return 0;
}
