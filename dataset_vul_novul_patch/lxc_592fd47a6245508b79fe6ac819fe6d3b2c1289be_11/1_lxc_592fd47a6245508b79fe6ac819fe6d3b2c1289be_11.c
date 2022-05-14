static int setup_ttydir_console(const struct lxc_rootfs *rootfs,
			 const struct lxc_console *console,
			 char *ttydir)
{
	char path[MAXPATHLEN], lxcpath[MAXPATHLEN];
	int ret;

	/* create rootfs/dev/<ttydir> directory */
	ret = snprintf(path, sizeof(path), "%s/dev/%s", rootfs->mount,
		       ttydir);
	if (ret >= sizeof(path))
		return -1;
	ret = mkdir(path, 0755);
	if (ret && errno != EEXIST) {
		SYSERROR("failed with errno %d to create %s", errno, path);
		return -1;
	}
	INFO("created %s", path);

	ret = snprintf(lxcpath, sizeof(lxcpath), "%s/dev/%s/console",
		       rootfs->mount, ttydir);
	if (ret >= sizeof(lxcpath)) {
		ERROR("console path too long");
		return -1;
	}

	snprintf(path, sizeof(path), "%s/dev/console", rootfs->mount);
	ret = unlink(path);
	if (ret && errno != ENOENT) {
		SYSERROR("error unlinking %s", path);
		return -1;
	}

	ret = creat(lxcpath, 0660);
	if (ret==-1 && errno != EEXIST) {
		SYSERROR("error %d creating %s", errno, lxcpath);
		return -1;
	}
	if (ret >= 0)
		close(ret);

	if (console->master < 0) {
		INFO("no console");
 		return 0;
 	}
 
	if (mount(console->name, lxcpath, "none", MS_BIND, 0)) {
 		ERROR("failed to mount '%s' on '%s'", console->name, lxcpath);
 		return -1;
 	}

	/* create symlink from rootfs/dev/console to 'lxc/console' */
	ret = snprintf(lxcpath, sizeof(lxcpath), "%s/console", ttydir);
	if (ret >= sizeof(lxcpath)) {
		ERROR("lxc/console path too long");
		return -1;
	}
	ret = symlink(lxcpath, path);
	if (ret) {
		SYSERROR("failed to create symlink for console");
		return -1;
	}

	INFO("console has been setup on %s", lxcpath);

	return 0;
}
