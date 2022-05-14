 static int cg_rmdir(const char *path)
 {
 	struct fuse_context *fc = fuse_get_context();
	char *fpath = NULL, *cgdir = NULL, *controller, *next = NULL;
 	const char *cgroup;
 	int ret;
 
	if (!fc)
		return -EIO;

	controller = pick_controller_from_path(fc, path);
	if (!controller)
		return -EINVAL;

	cgroup = find_cgroup_in_path(path);
	if (!cgroup)
		return -EINVAL;

	get_cgdir_and_path(cgroup, &cgdir, &fpath);
	if (!fpath) {
		ret = -EINVAL;
 		goto out;
 	}
 
	if (!caller_is_in_ancestor(fc->pid, controller, cgroup, &next)) {
		if (!fpath || strcmp(next, fpath) == 0)
			ret = -EBUSY;
		else
			ret = -ENOENT;
		goto out;
	}

 	if (!fc_may_access(fc, controller, cgdir, NULL, O_WRONLY)) {
 		ret = -EACCES;
 		goto out;
	}
	if (!caller_is_in_ancestor(fc->pid, controller, cgroup, NULL)) {
		ret = -EACCES;
		goto out;
	}

	if (!cgfs_remove(controller, cgroup)) {
		ret = -EINVAL;
		goto out;
	}

	ret = 0;
 
 out:
 	free(cgdir);
	free(next);
 	return ret;
 }
