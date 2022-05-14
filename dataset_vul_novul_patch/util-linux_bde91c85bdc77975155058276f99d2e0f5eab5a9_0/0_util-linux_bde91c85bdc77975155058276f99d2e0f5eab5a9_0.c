int xmkstemp(char **tmpname, char *dir)
int xmkstemp(char **tmpname, const char *dir, const char *prefix)
 {
 	char *localtmp;
	const char *tmpenv;
 	mode_t old_mode;
 	int fd, rc;
 
 	/* Some use cases must be capable of being moved atomically
 	 * with rename(2), which is the reason why dir is here.  */
	tmpenv = dir ? dir : getenv("TMPDIR");
	if (!tmpenv)
		tmpenv = _PATH_TMP;
 
	rc = asprintf(&localtmp, "%s/%s.XXXXXX", tmpenv, prefix);
 	if (rc < 0)
 		return -1;
 
	old_mode = umask(077);
	fd = mkostemp(localtmp, O_RDWR|O_CREAT|O_EXCL|O_CLOEXEC);
	umask(old_mode);
	if (fd == -1) {
		free(localtmp);
		localtmp = NULL;
	}
	*tmpname = localtmp;
	return fd;
}
