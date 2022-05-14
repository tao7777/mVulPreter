static int prepend_path(const struct path *path,
			const struct path *root,
			char **buffer, int *buflen)
{
	struct dentry *dentry;
	struct vfsmount *vfsmnt;
	struct mount *mnt;
	int error = 0;
	unsigned seq, m_seq = 0;
	char *bptr;
	int blen;

	rcu_read_lock();
restart_mnt:
	read_seqbegin_or_lock(&mount_lock, &m_seq);
	seq = 0;
	rcu_read_lock();
restart:
	bptr = *buffer;
	blen = *buflen;
	error = 0;
	dentry = path->dentry;
	vfsmnt = path->mnt;
	mnt = real_mount(vfsmnt);
	read_seqbegin_or_lock(&rename_lock, &seq);
	while (dentry != root->dentry || vfsmnt != root->mnt) {
		struct dentry * parent;
 
 		if (dentry == vfsmnt->mnt_root || IS_ROOT(dentry)) {
 			struct mount *parent = ACCESS_ONCE(mnt->mnt_parent);
			/* Escaped? */
			if (dentry != vfsmnt->mnt_root) {
				bptr = *buffer;
				blen = *buflen;
				error = 3;
				break;
			}
 			/* Global root? */
 			if (mnt != parent) {
 				dentry = ACCESS_ONCE(mnt->mnt_mountpoint);
				mnt = parent;
				vfsmnt = &mnt->mnt;
				continue;
			}
			if (!error)
				error = is_mounted(vfsmnt) ? 1 : 2;
			break;
		}
		parent = dentry->d_parent;
		prefetch(parent);
		error = prepend_name(&bptr, &blen, &dentry->d_name);
		if (error)
			break;

		dentry = parent;
	}
	if (!(seq & 1))
		rcu_read_unlock();
	if (need_seqretry(&rename_lock, seq)) {
		seq = 1;
		goto restart;
	}
	done_seqretry(&rename_lock, seq);

	if (!(m_seq & 1))
		rcu_read_unlock();
	if (need_seqretry(&mount_lock, m_seq)) {
		m_seq = 1;
		goto restart_mnt;
	}
	done_seqretry(&mount_lock, m_seq);

	if (error >= 0 && bptr == *buffer) {
		if (--blen < 0)
			error = -ENAMETOOLONG;
		else
			*--bptr = '/';
	}
	*buffer = bptr;
	*buflen = blen;
	return error;
}
