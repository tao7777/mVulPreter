static void nfs_set_open_stateid(struct nfs4_state *state, nfs4_stateid *stateid, int open_flags)
static void nfs_set_open_stateid(struct nfs4_state *state, nfs4_stateid *stateid, fmode_t fmode)
 {
 	write_seqlock(&state->seqlock);
	nfs_set_open_stateid_locked(state, stateid, fmode);
 	write_sequnlock(&state->seqlock);
 }
