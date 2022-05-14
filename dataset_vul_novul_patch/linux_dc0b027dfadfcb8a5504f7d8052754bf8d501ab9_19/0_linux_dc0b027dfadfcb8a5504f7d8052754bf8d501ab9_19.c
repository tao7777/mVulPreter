nfs4_proc_create(struct inode *dir, struct dentry *dentry, struct iattr *sattr,
                 int flags, struct nameidata *nd)
{
	struct path path = {
		.mnt = nd->path.mnt,
		.dentry = dentry,
 	};
 	struct nfs4_state *state;
 	struct rpc_cred *cred;
	fmode_t fmode = flags & (FMODE_READ | FMODE_WRITE);
 	int status = 0;
 
 	cred = rpc_lookup_cred();
 	if (IS_ERR(cred)) {
 		status = PTR_ERR(cred);
 		goto out;
 	}
	state = nfs4_do_open(dir, &path, fmode, flags, sattr, cred);
 	d_drop(dentry);
 	if (IS_ERR(state)) {
 		status = PTR_ERR(state);
		goto out_putcred;
	}
	d_add(dentry, igrab(state->inode));
	nfs_set_verifier(dentry, nfs_save_change_attribute(dir));
	if (flags & O_EXCL) {
		struct nfs_fattr fattr;
		status = nfs4_do_setattr(state->inode, cred, &fattr, sattr, state);
		if (status == 0)
			nfs_setattr_update_inode(state->inode, sattr);
 		nfs_post_op_update_inode(state->inode, &fattr);
 	}
 	if (status == 0 && (nd->flags & LOOKUP_OPEN) != 0)
		status = nfs4_intent_set_file(nd, &path, state, fmode);
 	else
		nfs4_close_sync(&path, state, fmode);
 out_putcred:
 	put_rpccred(cred);
 out:
	return status;
}
