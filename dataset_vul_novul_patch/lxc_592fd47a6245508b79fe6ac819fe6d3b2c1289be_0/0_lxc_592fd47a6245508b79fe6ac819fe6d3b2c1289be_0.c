static bool cgm_bind_dir(const char *root, const char *dirname)
{
	nih_local char *cgpath = NULL;

	/* /sys should have been mounted by now */
	cgpath = NIH_MUST( nih_strdup(NULL, root) );
	NIH_MUST( nih_strcat(&cgpath, NULL, "/sys/fs/cgroup") );

	if (!dir_exists(cgpath)) {
		ERROR("%s does not exist", cgpath);
		return false;
 	}
 
 	/* mount a tmpfs there so we can create subdirs */
	if (safe_mount("cgroup", cgpath, "tmpfs", 0, "size=10000,mode=755", root)) {
 		SYSERROR("Failed to mount tmpfs at %s", cgpath);
 		return false;
 	}
	NIH_MUST( nih_strcat(&cgpath, NULL, "/cgmanager") );

	if (mkdir(cgpath, 0755) < 0) {
		SYSERROR("Failed to create %s", cgpath);
 		return false;
 	}
 
	if (safe_mount(dirname, cgpath, "none", MS_BIND, 0, root)) {
 		SYSERROR("Failed to bind mount %s to %s", dirname, cgpath);
 		return false;
 	}

	return true;
}
