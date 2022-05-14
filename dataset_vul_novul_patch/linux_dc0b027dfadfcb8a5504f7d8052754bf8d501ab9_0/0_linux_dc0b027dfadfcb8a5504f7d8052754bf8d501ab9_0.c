static void __update_open_stateid(struct nfs4_state *state, nfs4_stateid *open_stateid, const nfs4_stateid *deleg_stateid, int open_flags)
static void __update_open_stateid(struct nfs4_state *state, nfs4_stateid *open_stateid, const nfs4_stateid *deleg_stateid, fmode_t fmode)
 {
 	/*
 	 * Protect the call to nfs4_state_set_mode_locked and
	 * serialise the stateid update
	 */
	write_seqlock(&state->seqlock);
	if (deleg_stateid != NULL) {
		memcpy(state->stateid.data, deleg_stateid->data, sizeof(state->stateid.data));
 		set_bit(NFS_DELEGATED_STATE, &state->flags);
 	}
 	if (open_stateid != NULL)
		nfs_set_open_stateid_locked(state, open_stateid, fmode);
 	write_sequnlock(&state->seqlock);
 	spin_lock(&state->owner->so_lock);
	update_open_stateflags(state, fmode);
 	spin_unlock(&state->owner->so_lock);
 }
