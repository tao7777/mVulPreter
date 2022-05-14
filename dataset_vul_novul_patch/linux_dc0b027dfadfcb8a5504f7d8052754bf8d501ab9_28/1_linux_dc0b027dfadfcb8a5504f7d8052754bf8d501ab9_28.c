void nfs4_close_sync(struct path *path, struct nfs4_state *state, mode_t mode)
 {
	__nfs4_close(path, state, mode, 1);
 }
