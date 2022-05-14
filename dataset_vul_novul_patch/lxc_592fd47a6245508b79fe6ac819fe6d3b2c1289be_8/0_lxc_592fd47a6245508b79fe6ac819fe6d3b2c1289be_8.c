 static int mount_entry_on_relative_rootfs(struct mntent *mntent,
					  const char *rootfs)
{
	char path[MAXPATHLEN];
	int ret;

	/* relative to root mount point */
	ret = snprintf(path, sizeof(path), "%s/%s", rootfs, mntent->mnt_dir);
	if (ret >= sizeof(path)) {
		ERROR("path name too long");
 		return -1;
 	}
 
	return mount_entry_on_generic(mntent, path, rootfs);
 }
