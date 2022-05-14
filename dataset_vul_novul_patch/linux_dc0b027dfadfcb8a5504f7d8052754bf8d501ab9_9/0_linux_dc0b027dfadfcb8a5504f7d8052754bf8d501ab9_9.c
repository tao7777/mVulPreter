static struct nfs4_state *nfs4_do_open(struct inode *dir, struct path *path, int flags, struct iattr *sattr, struct rpc_cred *cred)
static struct nfs4_state *nfs4_do_open(struct inode *dir, struct path *path, fmode_t fmode, int flags, struct iattr *sattr, struct rpc_cred *cred)
 {
 	struct nfs4_exception exception = { };
 	struct nfs4_state *res;
 	int status;
 
 	do {
		status = _nfs4_do_open(dir, path, fmode, flags, sattr, cred, &res);
 		if (status == 0)
 			break;
 		/* NOTE: BAD_SEQID means the server and client disagree about the
		 * book-keeping w.r.t. state-changing operations
		 * (OPEN/CLOSE/LOCK/LOCKU...)
		 * It is actually a sign of a bug on the client or on the server.
		 *
		 * If we receive a BAD_SEQID error in the particular case of
		 * doing an OPEN, we assume that nfs_increment_open_seqid() will
		 * have unhashed the old state_owner for us, and that we can
		 * therefore safely retry using a new one. We should still warn
		 * the user though...
		 */
		if (status == -NFS4ERR_BAD_SEQID) {
			printk(KERN_WARNING "NFS: v4 server %s "
					" returned a bad sequence-id error!\n",
					NFS_SERVER(dir)->nfs_client->cl_hostname);
			exception.retry = 1;
			continue;
		}
		/*
		 * BAD_STATEID on OPEN means that the server cancelled our
		 * state before it received the OPEN_CONFIRM.
		 * Recover by retrying the request as per the discussion
		 * on Page 181 of RFC3530.
		 */
		if (status == -NFS4ERR_BAD_STATEID) {
			exception.retry = 1;
			continue;
		}
		if (status == -EAGAIN) {
			/* We must have found a delegation */
			exception.retry = 1;
			continue;
		}
		res = ERR_PTR(nfs4_handle_exception(NFS_SERVER(dir),
					status, &exception));
	} while (exception.retry);
	return res;
}
