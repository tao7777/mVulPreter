static int can_open_cached(struct nfs4_state *state, int mode)
 {
 	int ret = 0;
	switch (mode & (FMODE_READ|FMODE_WRITE|O_EXCL)) {
 		case FMODE_READ:
 			ret |= test_bit(NFS_O_RDONLY_STATE, &state->flags) != 0;
 			break;
		case FMODE_WRITE:
			ret |= test_bit(NFS_O_WRONLY_STATE, &state->flags) != 0;
			break;
 		case FMODE_READ|FMODE_WRITE:
 			ret |= test_bit(NFS_O_RDWR_STATE, &state->flags) != 0;
 	}
 	return ret;
 }
