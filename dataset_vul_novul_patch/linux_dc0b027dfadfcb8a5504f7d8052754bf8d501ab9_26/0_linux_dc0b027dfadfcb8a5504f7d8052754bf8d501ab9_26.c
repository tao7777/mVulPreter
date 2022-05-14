static void __nfs4_close(struct path *path, struct nfs4_state *state, mode_t mode, int wait)
static void __nfs4_close(struct path *path, struct nfs4_state *state, fmode_t fmode, int wait)
 {
 	struct nfs4_state_owner *owner = state->owner;
 	int call_close = 0;
	fmode_t newstate;
 
 	atomic_inc(&owner->so_count);
 	/* Protect against nfs4_find_state() */
 	spin_lock(&owner->so_lock);
	switch (fmode & (FMODE_READ | FMODE_WRITE)) {
 		case FMODE_READ:
 			state->n_rdonly--;
 			break;
		case FMODE_WRITE:
			state->n_wronly--;
			break;
		case FMODE_READ|FMODE_WRITE:
			state->n_rdwr--;
	}
	newstate = FMODE_READ|FMODE_WRITE;
	if (state->n_rdwr == 0) {
		if (state->n_rdonly == 0) {
			newstate &= ~FMODE_READ;
			call_close |= test_bit(NFS_O_RDONLY_STATE, &state->flags);
			call_close |= test_bit(NFS_O_RDWR_STATE, &state->flags);
		}
		if (state->n_wronly == 0) {
			newstate &= ~FMODE_WRITE;
			call_close |= test_bit(NFS_O_WRONLY_STATE, &state->flags);
			call_close |= test_bit(NFS_O_RDWR_STATE, &state->flags);
		}
		if (newstate == 0)
			clear_bit(NFS_DELEGATED_STATE, &state->flags);
	}
	nfs4_state_set_mode_locked(state, newstate);
	spin_unlock(&owner->so_lock);

	if (!call_close) {
		nfs4_put_open_state(state);
		nfs4_put_state_owner(owner);
	} else
 		nfs4_do_close(path, state, wait);
 }
