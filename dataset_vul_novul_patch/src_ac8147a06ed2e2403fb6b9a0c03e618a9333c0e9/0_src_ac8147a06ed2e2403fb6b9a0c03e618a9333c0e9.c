sshkey_load_file(int fd, struct sshbuf *blob)
{
 	u_char buf[1024];
 	size_t len;
 	struct stat st;
	int r, dontmax = 0;
 
 	if (fstat(fd, &st) < 0)
 		return SSH_ERR_SYSTEM_ERROR;
 	if ((st.st_mode & (S_IFSOCK|S_IFCHR|S_IFIFO)) == 0 &&
 	    st.st_size > MAX_KEY_FILE_SIZE)
 		return SSH_ERR_INVALID_FORMAT;
	/*
	 * Pre-allocate the buffer used for the key contents and clamp its
	 * maximum size. This ensures that key contents are never leaked via
	 * implicit realloc() in the sshbuf code.
	 */
	if ((st.st_mode & S_IFREG) == 0 || st.st_size <= 0) {
		st.st_size = 64*1024; /* 64k should be enough for anyone :) */
		dontmax = 1;
	}
	if ((r = sshbuf_allocate(blob, st.st_size)) != 0 ||
	    (dontmax && (r = sshbuf_set_max_size(blob, st.st_size)) != 0))
		return r;
 	for (;;) {
 		if ((len = atomicio(read, fd, buf, sizeof(buf))) == 0) {
 			if (errno == EPIPE)
				break;
			r = SSH_ERR_SYSTEM_ERROR;
			goto out;
		}
		if ((r = sshbuf_put(blob, buf, len)) != 0)
			goto out;
		if (sshbuf_len(blob) > MAX_KEY_FILE_SIZE) {
			r = SSH_ERR_INVALID_FORMAT;
			goto out;
		}
	}
	if ((st.st_mode & (S_IFSOCK|S_IFCHR|S_IFIFO)) == 0 &&
	    st.st_size != (off_t)sshbuf_len(blob)) {
		r = SSH_ERR_FILE_CHANGED;
		goto out;
	}
	r = 0;

 out:
	explicit_bzero(buf, sizeof(buf));
	if (r != 0)
		sshbuf_reset(blob);
	return r;
}
