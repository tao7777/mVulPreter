static struct nfs4_state *nfs4_opendata_to_nfs4_state(struct nfs4_opendata *data)
{
	struct inode *inode;
	struct nfs4_state *state = NULL;
	struct nfs_delegation *delegation;
	int ret;

	if (!data->rpc_done) {
		state = nfs4_try_open_cached(data);
		goto out;
	}

	ret = -EAGAIN;
	if (!(data->f_attr.valid & NFS_ATTR_FATTR))
		goto err;
	inode = nfs_fhget(data->dir->d_sb, &data->o_res.fh, &data->f_attr);
	ret = PTR_ERR(inode);
	if (IS_ERR(inode))
		goto err;
	ret = -ENOMEM;
	state = nfs4_get_open_state(inode, data->owner);
	if (state == NULL)
		goto err_put_inode;
	if (data->o_res.delegation_type != 0) {
		int delegation_flags = 0;

		rcu_read_lock();
		delegation = rcu_dereference(NFS_I(inode)->delegation);
		if (delegation)
			delegation_flags = delegation->flags;
		rcu_read_unlock();
		if ((delegation_flags & 1UL<<NFS_DELEGATION_NEED_RECLAIM) == 0)
			nfs_inode_set_delegation(state->inode,
					data->owner->so_cred,
					&data->o_res);
		else
			nfs_inode_reclaim_delegation(state->inode,
					data->owner->so_cred,
					&data->o_res);
 	}
 
 	update_open_stateid(state, &data->o_res.stateid, NULL,
			data->o_arg.fmode);
 	iput(inode);
 out:
 	return state;
err_put_inode:
	iput(inode);
err:
	return ERR_PTR(ret);
}
