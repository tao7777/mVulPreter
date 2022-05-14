 int cg_mkdir(const char *path, mode_t mode)
 {
 	struct fuse_context *fc = fuse_get_context();
	char *fpath = NULL, *path1, *cgdir = NULL, *controller;
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
	if (!fpath)
		path1 = "/";
 	else
 		path1 = cgdir;
 
 	if (!fc_may_access(fc, controller, path1, NULL, O_RDWR)) {
 		ret = -EACCES;
 		goto out;
	}
	if (!caller_is_in_ancestor(fc->pid, controller, path1, NULL)) {
		ret = -EACCES;
		goto out;
	}

	ret = cgfs_create(controller, cgroup, fc->uid, fc->gid);
	printf("cgfs_create returned %d for %s %s\n", ret, controller, cgroup);
 
 out:
 	free(cgdir);
 	return ret;
 }
