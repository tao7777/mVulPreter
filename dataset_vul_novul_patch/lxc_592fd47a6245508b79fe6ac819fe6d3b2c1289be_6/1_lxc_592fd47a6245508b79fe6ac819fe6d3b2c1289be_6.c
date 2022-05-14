 static int mount_entry_on_absolute_rootfs(struct mntent *mntent,
					  const struct lxc_rootfs *rootfs,
					  const char *lxc_name)
{
	char *aux;
	char path[MAXPATHLEN];
	int r, ret = 0, offset;
	const char *lxcpath;

	lxcpath = lxc_global_config_value("lxc.lxcpath");
	if (!lxcpath) {
		ERROR("Out of memory");
		return -1;
	}

	/* if rootfs->path is a blockdev path, allow container fstab to
	 * use $lxcpath/CN/rootfs as the target prefix */
	r = snprintf(path, MAXPATHLEN, "%s/%s/rootfs", lxcpath, lxc_name);
	if (r < 0 || r >= MAXPATHLEN)
		goto skipvarlib;

	aux = strstr(mntent->mnt_dir, path);
	if (aux) {
		offset = strlen(path);
		goto skipabs;
	}

skipvarlib:
	aux = strstr(mntent->mnt_dir, rootfs->path);
	if (!aux) {
		WARN("ignoring mount point '%s'", mntent->mnt_dir);
		return ret;
	}
	offset = strlen(rootfs->path);

skipabs:

	r = snprintf(path, MAXPATHLEN, "%s/%s", rootfs->mount,
		 aux + offset);
	if (r < 0 || r >= MAXPATHLEN) {
		WARN("pathnme too long for '%s'", mntent->mnt_dir);
 		return -1;
 	}
 
	return mount_entry_on_generic(mntent, path);
 }
