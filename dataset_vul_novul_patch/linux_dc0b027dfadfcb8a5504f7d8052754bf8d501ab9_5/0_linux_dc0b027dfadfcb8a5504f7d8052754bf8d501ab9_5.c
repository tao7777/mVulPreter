nfs4_atomic_open(struct inode *dir, struct dentry *dentry, struct nameidata *nd)
{
	struct path path = {
		.mnt = nd->path.mnt,
		.dentry = dentry,
	};
	struct dentry *parent;
	struct iattr attr;
 	struct rpc_cred *cred;
 	struct nfs4_state *state;
 	struct dentry *res;
	fmode_t fmode = nd->intent.open.flags & (FMODE_READ | FMODE_WRITE | FMODE_EXEC);
 
 	if (nd->flags & LOOKUP_CREATE) {
 		attr.ia_mode = nd->intent.open.create_mode;
		attr.ia_valid = ATTR_MODE;
		if (!IS_POSIXACL(dir))
			attr.ia_mode &= ~current->fs->umask;
	} else {
		attr.ia_valid = 0;
		BUG_ON(nd->intent.open.flags & O_CREAT);
	}

	cred = rpc_lookup_cred();
	if (IS_ERR(cred))
		return (struct dentry *)cred;
 	parent = dentry->d_parent;
 	/* Protect against concurrent sillydeletes */
 	nfs_block_sillyrename(parent);
	state = nfs4_do_open(dir, &path, fmode, nd->intent.open.flags, &attr, cred);
 	put_rpccred(cred);
 	if (IS_ERR(state)) {
 		if (PTR_ERR(state) == -ENOENT) {
			d_add(dentry, NULL);
			nfs_set_verifier(dentry, nfs_save_change_attribute(dir));
		}
		nfs_unblock_sillyrename(parent);
		return (struct dentry *)state;
	}
	res = d_add_unique(dentry, igrab(state->inode));
	if (res != NULL)
 		path.dentry = res;
 	nfs_set_verifier(path.dentry, nfs_save_change_attribute(dir));
 	nfs_unblock_sillyrename(parent);
	nfs4_intent_set_file(nd, &path, state, fmode);
 	return res;
 }
