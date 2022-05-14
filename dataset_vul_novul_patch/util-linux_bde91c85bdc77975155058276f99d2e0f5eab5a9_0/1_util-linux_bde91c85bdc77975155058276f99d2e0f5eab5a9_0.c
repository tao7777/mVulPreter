int xmkstemp(char **tmpname, char *dir)
 {
 	char *localtmp;
	char *tmpenv;
 	mode_t old_mode;
 	int fd, rc;
 
 	/* Some use cases must be capable of being moved atomically
 	 * with rename(2), which is the reason why dir is here.  */
	if (dir != NULL)
		tmpenv = dir;
	else
		tmpenv = getenv("TMPDIR");
	if (tmpenv)
		rc = asprintf(&localtmp, "%s/%s.XXXXXX", tmpenv,
			  program_invocation_short_name);
	else
		rc = asprintf(&localtmp, "%s/%s.XXXXXX", _PATH_TMP,
			  program_invocation_short_name);
 
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
