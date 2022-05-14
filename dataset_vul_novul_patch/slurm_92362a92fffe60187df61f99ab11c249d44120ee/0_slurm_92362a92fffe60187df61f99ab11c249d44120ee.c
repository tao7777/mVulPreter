 _prolog_error(batch_job_launch_msg_t *req, int rc)
 {
	char *err_name_ptr, err_name[256], path_name[MAXPATHLEN];
	char *fmt_char;
	int fd;

	if (req->std_err || req->std_out) {
		if (req->std_err)
			strncpy(err_name, req->std_err, sizeof(err_name));
		else
			strncpy(err_name, req->std_out, sizeof(err_name));
		if ((fmt_char = strchr(err_name, (int) '%')) &&
		    (fmt_char[1] == 'j') && !strchr(fmt_char+1, (int) '%')) {
			char tmp_name[256];
			fmt_char[1] = 'u';
			snprintf(tmp_name, sizeof(tmp_name), err_name,
				 req->job_id);
			strncpy(err_name, tmp_name, sizeof(err_name));
		}
	} else {
		snprintf(err_name, sizeof(err_name), "slurm-%u.out",
			 req->job_id);
	}
	err_name_ptr = err_name;
	if (err_name_ptr[0] == '/')
		snprintf(path_name, MAXPATHLEN, "%s", err_name_ptr);
	else if (req->work_dir)
		snprintf(path_name, MAXPATHLEN, "%s/%s",
 			req->work_dir, err_name_ptr);
 	else
 		snprintf(path_name, MAXPATHLEN, "/%s", err_name_ptr);
	if ((fd = _open_as_other(path_name, req)) == -1) {
		error("Unable to open %s: Permission denied", path_name);
 		return;
 	}
 	snprintf(err_name, sizeof(err_name),
		 "Error running slurm prolog: %d\n", WEXITSTATUS(rc));
	safe_write(fd, err_name, strlen(err_name));
	if (fchown(fd, (uid_t) req->uid, (gid_t) req->gid) == -1) {
		snprintf(err_name, sizeof(err_name),
			 "Couldn't change fd owner to %u:%u: %m\n",
			 req->uid, req->gid);
	}
rwfail:
	close(fd);
}
