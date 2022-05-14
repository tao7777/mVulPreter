int cg_write(const char *path, const char *buf, size_t size, off_t offset,
	     struct fuse_file_info *fi)
{
	struct fuse_context *fc = fuse_get_context();
	char *localbuf = NULL;
	struct cgfs_files *k = NULL;
	struct file_info *f = (struct file_info *)fi->fh;
	bool r;

	if (f->type != LXC_TYPE_CGFILE) {
		fprintf(stderr, "Internal error: directory cache info used in cg_write\n");
		return -EIO;
	}

	if (offset)
		return 0;

	if (!fc)
		return -EIO;

	localbuf = alloca(size+1);
	localbuf[size] = '\0';
	memcpy(localbuf, buf, size);

	if ((k = cgfs_get_key(f->controller, f->cgroup, f->file)) == NULL) {
		size = -EINVAL;
		goto out;
	}

	if (!fc_may_access(fc, f->controller, f->cgroup, f->file, O_WRONLY)) {
		size = -EACCES;
		goto out;
	}

	if (strcmp(f->file, "tasks") == 0 ||
			strcmp(f->file, "/tasks") == 0 ||
 			strcmp(f->file, "/cgroup.procs") == 0 ||
 			strcmp(f->file, "cgroup.procs") == 0)
		r = do_write_pids(fc->pid, fc->uid, f->controller, f->cgroup, f->file, localbuf);
 	else
 		r = cgfs_set_value(f->controller, f->cgroup, f->file, localbuf);
 
	if (!r)
		size = -EINVAL;

out:
	free_key(k);
	return size;
}
