static int cg_open(const char *path, struct fuse_file_info *fi)
{
	const char *cgroup;
	char *fpath = NULL, *path1, *path2, * cgdir = NULL, *controller;
	struct cgfs_files *k = NULL;
	struct file_info *file_info;
	struct fuse_context *fc = fuse_get_context();
	int ret;

	if (!fc)
		return -EIO;

	controller = pick_controller_from_path(fc, path);
	if (!controller)
		return -EIO;
	cgroup = find_cgroup_in_path(path);
	if (!cgroup)
		return -EINVAL;

	get_cgdir_and_path(cgroup, &cgdir, &fpath);
	if (!fpath) {
		path1 = "/";
		path2 = cgdir;
	} else {
		path1 = cgdir;
		path2 = fpath;
	}

	k = cgfs_get_key(controller, path1, path2);
	if (!k) {
		ret = -EINVAL;
		goto out;
 	}
 	free_key(k);
 
	if (!caller_may_see_dir(fc->pid, controller, path1)) {
		ret = -ENOENT;
		goto out;
	}
 	if (!fc_may_access(fc, controller, path1, path2, fi->flags)) {
 		ret = -EACCES;
		goto out;
	}

	/* we'll free this at cg_release */
	file_info = malloc(sizeof(*file_info));
	if (!file_info) {
		ret = -ENOMEM;
		goto out;
	}
	file_info->controller = must_copy_string(controller);
	file_info->cgroup = must_copy_string(path1);
	file_info->file = must_copy_string(path2);
	file_info->type = LXC_TYPE_CGFILE;
	file_info->buf = NULL;
	file_info->buflen = 0;

	fi->fh = (unsigned long)file_info;
	ret = 0;

out:
	free(cgdir);
	return ret;
}
