static int cg_opendir(const char *path, struct fuse_file_info *fi)
{
	struct fuse_context *fc = fuse_get_context();
	const char *cgroup;
	struct file_info *dir_info;
	char *controller = NULL;

	if (!fc)
		return -EIO;

	if (strcmp(path, "/cgroup") == 0) {
		cgroup = NULL;
		controller = NULL;
	} else {
		controller = pick_controller_from_path(fc, path);
		if (!controller)
			return -EIO;

		cgroup = find_cgroup_in_path(path);
		if (!cgroup) {
			/* this is just /cgroup/controller, return its contents */
			cgroup = "/";
 		}
 	}
 
	if (cgroup) {
		if (!caller_may_see_dir(fc->pid, controller, cgroup))
			return -ENOENT;
		if (!fc_may_access(fc, controller, cgroup, NULL, O_RDONLY))
			return -EACCES;
 	}
 
 	/* we'll free this at cg_releasedir */
	dir_info = malloc(sizeof(*dir_info));
	if (!dir_info)
		return -ENOMEM;
	dir_info->controller = must_copy_string(controller);
	dir_info->cgroup = must_copy_string(cgroup);
	dir_info->type = LXC_TYPE_CGDIR;
	dir_info->buf = NULL;
	dir_info->file = NULL;
	dir_info->buflen = 0;

	fi->fh = (unsigned long)dir_info;
	return 0;
}
