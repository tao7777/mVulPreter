static int mount_autodev(const char *name, const struct lxc_rootfs *rootfs, const char *lxcpath)
{
	int ret;
	size_t clen;
	char *path;

	INFO("Mounting container /dev");

	/* $(rootfs->mount) + "/dev/pts" + '\0' */
	clen = (rootfs->path ? strlen(rootfs->mount) : 0) + 9;
	path = alloca(clen);

	ret = snprintf(path, clen, "%s/dev", rootfs->path ? rootfs->mount : "");
	if (ret < 0 || ret >= clen)
		return -1;

	if (!dir_exists(path)) {
		WARN("No /dev in container.");
		WARN("Proceeding without autodev setup");
 		return 0;
 	}
 
	if (safe_mount("none", path, "tmpfs", 0, "size=100000,mode=755",
				rootfs->path ? rootfs->mount : NULL)) {
 		SYSERROR("Failed mounting tmpfs onto %s\n", path);
 		return false;
 	}

	INFO("Mounted tmpfs onto %s",  path);

	ret = snprintf(path, clen, "%s/dev/pts", rootfs->path ? rootfs->mount : "");
	if (ret < 0 || ret >= clen)
		return -1;

	/*
	 * If we are running on a devtmpfs mapping, dev/pts may already exist.
	 * If not, then create it and exit if that fails...
	 */
	if (!dir_exists(path)) {
		ret = mkdir(path, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
		if (ret) {
			SYSERROR("Failed to create /dev/pts in container");
			return -1;
		}
	}

	INFO("Mounted container /dev");
	return 0;
}
