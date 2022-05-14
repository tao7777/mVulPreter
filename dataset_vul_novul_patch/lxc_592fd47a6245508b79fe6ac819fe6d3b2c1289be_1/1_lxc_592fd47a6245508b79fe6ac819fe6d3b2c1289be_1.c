static int fill_autodev(const struct lxc_rootfs *rootfs)
{
	int ret;
	char path[MAXPATHLEN];
	int i;
	mode_t cmask;

	INFO("Creating initial consoles under container /dev");

	ret = snprintf(path, MAXPATHLEN, "%s/dev", rootfs->path ? rootfs->mount : "");
	if (ret < 0 || ret >= MAXPATHLEN) {
		ERROR("Error calculating container /dev location");
		return -1;
	}

	if (!dir_exists(path))  // ignore, just don't try to fill in
		return 0;

	INFO("Populating container /dev");
	cmask = umask(S_IXUSR | S_IXGRP | S_IXOTH);
	for (i = 0; i < sizeof(lxc_devs) / sizeof(lxc_devs[0]); i++) {
		const struct lxc_devs *d = &lxc_devs[i];
		ret = snprintf(path, MAXPATHLEN, "%s/dev/%s", rootfs->path ? rootfs->mount : "", d->name);
		if (ret < 0 || ret >= MAXPATHLEN)
			return -1;
		ret = mknod(path, d->mode, makedev(d->maj, d->min));
		if (ret && errno != EEXIST) {
			char hostpath[MAXPATHLEN];
			FILE *pathfile;

			ret = snprintf(hostpath, MAXPATHLEN, "/dev/%s", d->name);
			if (ret < 0 || ret >= MAXPATHLEN)
				return -1;
			pathfile = fopen(path, "wb");
			if (!pathfile) {
				SYSERROR("Failed to create device mount target '%s'", path);
 				return -1;
 			}
 			fclose(pathfile);
			if (mount(hostpath, path, 0, MS_BIND, NULL) != 0) {
 				SYSERROR("Failed bind mounting device %s from host into container",
 					d->name);
 				return -1;
			}
		}
	}
	umask(cmask);

	INFO("Populated container /dev");
	return 0;
}
