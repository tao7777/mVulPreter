static char *lxclock_name(const char *p, const char *n)
{
	int ret;
	int len;
	char *dest;
 	char *rundir;
 
 	/* lockfile will be:
	 * "/run" + "/lxc/lock/$lxcpath/$lxcname + '\0' if root
 	 * or
	 * $XDG_RUNTIME_DIR + "/lxc/lock/$lxcpath/$lxcname + '\0' if non-root
 	 */
 
	/* length of "/lxc/lock/" + $lxcpath + "/" + "." + $lxcname + '\0' */
	len = strlen("/lxc/lock/") + strlen(n) + strlen(p) + 3;
 	rundir = get_rundir();
 	if (!rundir)
 		return NULL;
	len += strlen(rundir);

	if ((dest = malloc(len)) == NULL) {
		free(rundir);
 		return NULL;
 	}
 
	ret = snprintf(dest, len, "%s/lxc/lock/%s", rundir, p);
 	if (ret < 0 || ret >= len) {
 		free(dest);
 		free(rundir);
 		return NULL;
 	}
 	ret = mkdir_p(dest, 0755);
 	if (ret < 0) {
		free(dest);
		free(rundir);
		return NULL;
	}
 
	ret = snprintf(dest, len, "%s/lxc/lock/%s/.%s", rundir, p, n);
 	free(rundir);
 	if (ret < 0 || ret >= len) {
 		free(dest);
 		return NULL;
	}
	return dest;
}
