static char *lxclock_name(const char *p, const char *n)
{
	int ret;
	int len;
	char *dest;
 	char *rundir;
 
 	/* lockfile will be:
	 * "/run" + "/lock/lxc/$lxcpath/$lxcname + '\0' if root
 	 * or
	 * $XDG_RUNTIME_DIR + "/lock/lxc/$lxcpath/$lxcname + '\0' if non-root
 	 */
 
	/* length of "/lock/lxc/" + $lxcpath + "/" + "." + $lxcname + '\0' */
	len = strlen("/lock/lxc/") + strlen(n) + strlen(p) + 3;
 	rundir = get_rundir();
 	if (!rundir)
 		return NULL;
	len += strlen(rundir);

	if ((dest = malloc(len)) == NULL) {
		free(rundir);
 		return NULL;
 	}
 
	ret = snprintf(dest, len, "%s/lock/lxc/%s", rundir, p);
 	if (ret < 0 || ret >= len) {
 		free(dest);
 		free(rundir);
 		return NULL;
 	}
 	ret = mkdir_p(dest, 0755);
 	if (ret < 0) {
		/* fall back to "/tmp/" + $(id -u) + "/lxc" + $lxcpath + "/" + "." + $lxcname + '\0'
		 * * maximum length of $(id -u) is 10 calculated by (log (2 ** (sizeof(uid_t) * 8) - 1) / log 10 + 1)
		 * * lxcpath always starts with '/'
		 */
		int l2 = 22 + strlen(n) + strlen(p);
		if (l2 > len) {
			char *d;
			d = realloc(dest, l2);
			if (!d) {
				free(dest);
				free(rundir);
				return NULL;
			}
			len = l2;
			dest = d;
		}
		ret = snprintf(dest, len, "/tmp/%d/lxc%s", geteuid(), p);
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
		ret = snprintf(dest, len, "/tmp/%d/lxc%s/.%s", geteuid(), p, n);
	} else
		ret = snprintf(dest, len, "%s/lock/lxc/%s/.%s", rundir, p, n);
 
 	free(rundir);
 	if (ret < 0 || ret >= len) {
 		free(dest);
 		return NULL;
	}
	return dest;
}
