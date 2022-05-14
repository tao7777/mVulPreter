 int propagate_mnt(struct mount *dest_mnt, struct dentry *dest_dentry,
 		    struct mount *source_mnt, struct list_head *tree_list)
 {
	struct user_namespace *user_ns = current->nsproxy->mnt_ns->user_ns;
 	struct mount *m, *child;
 	int ret = 0;
 	struct mount *prev_dest_mnt = dest_mnt;
	struct mount *prev_src_mnt  = source_mnt;
	LIST_HEAD(tmp_list);
	LIST_HEAD(umount_list);

	for (m = propagation_next(dest_mnt, dest_mnt); m;
			m = propagation_next(m, dest_mnt)) {
		int type;
		struct mount *source;

		if (IS_MNT_NEW(m))
			continue;
 
 		source =  get_source(m, prev_dest_mnt, prev_src_mnt, &type);
 
		/* Notice when we are propagating across user namespaces */
		if (m->mnt_ns->user_ns != user_ns)
			type |= CL_UNPRIVILEGED;

 		child = copy_tree(source, source->mnt.mnt_root, type);
 		if (IS_ERR(child)) {
 			ret = PTR_ERR(child);
			list_splice(tree_list, tmp_list.prev);
			goto out;
		}

		if (is_subdir(dest_dentry, m->mnt.mnt_root)) {
			mnt_set_mountpoint(m, dest_dentry, child);
			list_add_tail(&child->mnt_hash, tree_list);
		} else {
			/*
			 * This can happen if the parent mount was bind mounted
			 * on some subdirectory of a shared/slave mount.
			 */
			list_add_tail(&child->mnt_hash, &tmp_list);
		}
		prev_dest_mnt = m;
		prev_src_mnt  = child;
	}
out:
	br_write_lock(&vfsmount_lock);
	while (!list_empty(&tmp_list)) {
		child = list_first_entry(&tmp_list, struct mount, mnt_hash);
		umount_tree(child, 0, &umount_list);
	}
	br_write_unlock(&vfsmount_lock);
	release_mounts(&umount_list);
	return ret;
}
