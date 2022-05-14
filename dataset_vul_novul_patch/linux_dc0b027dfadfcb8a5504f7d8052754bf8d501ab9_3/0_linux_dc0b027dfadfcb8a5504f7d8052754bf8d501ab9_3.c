static int can_open_cached(struct nfs4_state *state, int mode)
static int can_open_cached(struct nfs4_state *state, fmode_t mode, int open_mode)
 {
 	int ret = 0;

	if (open_mode & O_EXCL)
		goto out;
	switch (mode & (FMODE_READ|FMODE_WRITE)) {
 		case FMODE_READ:
 			ret |= test_bit(NFS_O_RDONLY_STATE, &state->flags) != 0;
 			break;
		case FMODE_WRITE:
			ret |= test_bit(NFS_O_WRONLY_STATE, &state->flags) != 0;
			break;
 		case FMODE_READ|FMODE_WRITE:
 			ret |= test_bit(NFS_O_RDWR_STATE, &state->flags) != 0;
 	}
out:
 	return ret;
 }
