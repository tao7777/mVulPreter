static struct nfs4_state *nfs4_try_open_cached(struct nfs4_opendata *opendata)
{
 	struct nfs4_state *state = opendata->state;
 	struct nfs_inode *nfsi = NFS_I(state->inode);
 	struct nfs_delegation *delegation;
	int open_mode = opendata->o_arg.open_flags & O_EXCL;
	fmode_t fmode = opendata->o_arg.fmode;
 	nfs4_stateid stateid;
 	int ret = -EAGAIN;
 
 	for (;;) {
		if (can_open_cached(state, fmode, open_mode)) {
 			spin_lock(&state->owner->so_lock);
			if (can_open_cached(state, fmode, open_mode)) {
				update_open_stateflags(state, fmode);
 				spin_unlock(&state->owner->so_lock);
 				goto out_return_state;
 			}
			spin_unlock(&state->owner->so_lock);
		}
 		rcu_read_lock();
 		delegation = rcu_dereference(nfsi->delegation);
 		if (delegation == NULL ||
		    !can_open_delegated(delegation, fmode)) {
 			rcu_read_unlock();
 			break;
 		}
		/* Save the delegation */
		memcpy(stateid.data, delegation->stateid.data, sizeof(stateid.data));
		rcu_read_unlock();
		ret = nfs_may_open(state->inode, state->owner->so_cred, open_mode);
		if (ret != 0)
			goto out;
 		ret = -EAGAIN;
 
 		/* Try to update the stateid using the delegation */
		if (update_open_stateid(state, NULL, &stateid, fmode))
 			goto out_return_state;
 	}
 out:
	return ERR_PTR(ret);
out_return_state:
	atomic_inc(&state->count);
	return state;
}
