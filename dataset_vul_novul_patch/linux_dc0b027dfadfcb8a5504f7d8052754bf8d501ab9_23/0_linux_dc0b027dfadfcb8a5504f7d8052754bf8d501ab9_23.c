static void nfs_set_open_stateid_locked(struct nfs4_state *state, nfs4_stateid *stateid, int open_flags)
static void nfs_set_open_stateid_locked(struct nfs4_state *state, nfs4_stateid *stateid, fmode_t fmode)
 {
 	if (test_bit(NFS_DELEGATED_STATE, &state->flags) == 0)
 		memcpy(state->stateid.data, stateid->data, sizeof(state->stateid.data));
 	memcpy(state->open_stateid.data, stateid->data, sizeof(state->open_stateid.data));
	switch (fmode) {
 		case FMODE_READ:
 			set_bit(NFS_O_RDONLY_STATE, &state->flags);
 			break;
		case FMODE_WRITE:
			set_bit(NFS_O_WRONLY_STATE, &state->flags);
			break;
		case FMODE_READ|FMODE_WRITE:
			set_bit(NFS_O_RDWR_STATE, &state->flags);
 	}
 }
