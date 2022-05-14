void nfs4_close_sync(struct path *path, struct nfs4_state *state, mode_t mode)
void nfs4_close_sync(struct path *path, struct nfs4_state *state, fmode_t fmode)
 {
	__nfs4_close(path, state, fmode, 1);
 }
