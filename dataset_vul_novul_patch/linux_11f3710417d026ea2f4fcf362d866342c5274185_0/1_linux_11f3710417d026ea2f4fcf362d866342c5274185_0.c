static int ovl_rename2(struct inode *olddir, struct dentry *old,
		       struct inode *newdir, struct dentry *new,
		       unsigned int flags)
{
	int err;
	enum ovl_path_type old_type;
	enum ovl_path_type new_type;
	struct dentry *old_upperdir;
	struct dentry *new_upperdir;
	struct dentry *olddentry;
	struct dentry *newdentry;
	struct dentry *trap;
	bool old_opaque;
	bool new_opaque;
	bool new_create = false;
	bool cleanup_whiteout = false;
	bool overwrite = !(flags & RENAME_EXCHANGE);
	bool is_dir = d_is_dir(old);
	bool new_is_dir = false;
	struct dentry *opaquedir = NULL;
	const struct cred *old_cred = NULL;
	struct cred *override_cred = NULL;

	err = -EINVAL;
	if (flags & ~(RENAME_EXCHANGE | RENAME_NOREPLACE))
		goto out;

	flags &= ~RENAME_NOREPLACE;

	err = ovl_check_sticky(old);
	if (err)
		goto out;

	/* Don't copy up directory trees */
	old_type = ovl_path_type(old);
	err = -EXDEV;
	if (OVL_TYPE_MERGE_OR_LOWER(old_type) && is_dir)
		goto out;

	if (new->d_inode) {
		err = ovl_check_sticky(new);
		if (err)
			goto out;

		if (d_is_dir(new))
			new_is_dir = true;

		new_type = ovl_path_type(new);
		err = -EXDEV;
		if (!overwrite && OVL_TYPE_MERGE_OR_LOWER(new_type) && new_is_dir)
			goto out;

		err = 0;
		if (!OVL_TYPE_UPPER(new_type) && !OVL_TYPE_UPPER(old_type)) {
			if (ovl_dentry_lower(old)->d_inode ==
			    ovl_dentry_lower(new)->d_inode)
				goto out;
		}
		if (OVL_TYPE_UPPER(new_type) && OVL_TYPE_UPPER(old_type)) {
			if (ovl_dentry_upper(old)->d_inode ==
			    ovl_dentry_upper(new)->d_inode)
				goto out;
		}
	} else {
		if (ovl_dentry_is_opaque(new))
			new_type = __OVL_PATH_UPPER;
		else
			new_type = __OVL_PATH_UPPER | __OVL_PATH_PURE;
	}

	err = ovl_want_write(old);
	if (err)
		goto out;

	err = ovl_copy_up(old);
	if (err)
		goto out_drop_write;

	err = ovl_copy_up(new->d_parent);
	if (err)
		goto out_drop_write;
	if (!overwrite) {
		err = ovl_copy_up(new);
		if (err)
			goto out_drop_write;
	}

	old_opaque = !OVL_TYPE_PURE_UPPER(old_type);
	new_opaque = !OVL_TYPE_PURE_UPPER(new_type);

	if (old_opaque || new_opaque) {
		err = -ENOMEM;
		override_cred = prepare_creds();
		if (!override_cred)
			goto out_drop_write;

		/*
		 * CAP_SYS_ADMIN for setting xattr on whiteout, opaque dir
		 * CAP_DAC_OVERRIDE for create in workdir
		 * CAP_FOWNER for removing whiteout from sticky dir
		 * CAP_FSETID for chmod of opaque dir
		 * CAP_CHOWN for chown of opaque dir
		 */
		cap_raise(override_cred->cap_effective, CAP_SYS_ADMIN);
		cap_raise(override_cred->cap_effective, CAP_DAC_OVERRIDE);
		cap_raise(override_cred->cap_effective, CAP_FOWNER);
		cap_raise(override_cred->cap_effective, CAP_FSETID);
		cap_raise(override_cred->cap_effective, CAP_CHOWN);
		old_cred = override_creds(override_cred);
	}

	if (overwrite && OVL_TYPE_MERGE_OR_LOWER(new_type) && new_is_dir) {
		opaquedir = ovl_check_empty_and_clear(new);
		err = PTR_ERR(opaquedir);
		if (IS_ERR(opaquedir)) {
			opaquedir = NULL;
			goto out_revert_creds;
		}
	}

	if (overwrite) {
		if (old_opaque) {
			if (new->d_inode || !new_opaque) {
				/* Whiteout source */
				flags |= RENAME_WHITEOUT;
			} else {
				/* Switch whiteouts */
				flags |= RENAME_EXCHANGE;
			}
		} else if (is_dir && !new->d_inode && new_opaque) {
			flags |= RENAME_EXCHANGE;
			cleanup_whiteout = true;
		}
	}

	old_upperdir = ovl_dentry_upper(old->d_parent);
	new_upperdir = ovl_dentry_upper(new->d_parent);
 
 	trap = lock_rename(new_upperdir, old_upperdir);
 
	olddentry = ovl_dentry_upper(old);
	newdentry = ovl_dentry_upper(new);
	if (newdentry) {
 		if (opaquedir) {
			newdentry = opaquedir;
			opaquedir = NULL;
 		} else {
			dget(newdentry);
 		}
 	} else {
 		new_create = true;
		newdentry = lookup_one_len(new->d_name.name, new_upperdir,
					   new->d_name.len);
		err = PTR_ERR(newdentry);
		if (IS_ERR(newdentry))
			goto out_unlock;
 	}
 
	err = -ESTALE;
	if (olddentry->d_parent != old_upperdir)
		goto out_dput;
	if (newdentry->d_parent != new_upperdir)
		goto out_dput;
 	if (olddentry == trap)
 		goto out_dput;
 	if (newdentry == trap)
		goto out_dput;

	if (is_dir && !old_opaque && new_opaque) {
		err = ovl_set_opaque(olddentry);
		if (err)
			goto out_dput;
	}
	if (!overwrite && new_is_dir && old_opaque && !new_opaque) {
		err = ovl_set_opaque(newdentry);
		if (err)
			goto out_dput;
	}

	if (old_opaque || new_opaque) {
		err = ovl_do_rename(old_upperdir->d_inode, olddentry,
				    new_upperdir->d_inode, newdentry,
				    flags);
	} else {
		/* No debug for the plain case */
		BUG_ON(flags & ~RENAME_EXCHANGE);
		err = vfs_rename(old_upperdir->d_inode, olddentry,
				 new_upperdir->d_inode, newdentry,
				 NULL, flags);
	}

	if (err) {
		if (is_dir && !old_opaque && new_opaque)
			ovl_remove_opaque(olddentry);
		if (!overwrite && new_is_dir && old_opaque && !new_opaque)
			ovl_remove_opaque(newdentry);
		goto out_dput;
	}

	if (is_dir && old_opaque && !new_opaque)
		ovl_remove_opaque(olddentry);
	if (!overwrite && new_is_dir && !old_opaque && new_opaque)
		ovl_remove_opaque(newdentry);

	/*
	 * Old dentry now lives in different location. Dentries in
	 * lowerstack are stale. We cannot drop them here because
	 * access to them is lockless. This could be only pure upper
	 * or opaque directory - numlower is zero. Or upper non-dir
	 * entry - its pureness is tracked by flag opaque.
	 */
	if (old_opaque != new_opaque) {
		ovl_dentry_set_opaque(old, new_opaque);
		if (!overwrite)
			ovl_dentry_set_opaque(new, old_opaque);
	}

	if (cleanup_whiteout)
		ovl_cleanup(old_upperdir->d_inode, newdentry);

	ovl_dentry_version_inc(old->d_parent);
	ovl_dentry_version_inc(new->d_parent);
 
 out_dput:
 	dput(newdentry);
 out_unlock:
 	unlock_rename(new_upperdir, old_upperdir);
 out_revert_creds:
	if (old_opaque || new_opaque) {
		revert_creds(old_cred);
		put_cred(override_cred);
	}
out_drop_write:
	ovl_drop_write(old);
out:
	dput(opaquedir);
	return err;
}
