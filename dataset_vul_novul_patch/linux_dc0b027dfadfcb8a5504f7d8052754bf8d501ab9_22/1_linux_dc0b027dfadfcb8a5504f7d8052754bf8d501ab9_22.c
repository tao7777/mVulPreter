static void nfs_set_open_stateid(struct nfs4_state *state, nfs4_stateid *stateid, int open_flags)
 {
 	write_seqlock(&state->seqlock);
	nfs_set_open_stateid_locked(state, stateid, open_flags);
 	write_sequnlock(&state->seqlock);
 }
