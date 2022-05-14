static bool glfs_check_config(const char *cfgstring, char **reason)
{
	char *path;
	glfs_t *fs = NULL;
	glfs_fd_t *gfd = NULL;
	gluster_server *hosts = NULL; /* gluster server defination */
	bool result = true;
	path = strchr(cfgstring, '/');
	if (!path) {
		if (asprintf(reason, "No path found") == -1)
			*reason = NULL;
		result = false;
		goto done;
	}
	path += 1; /* get past '/' */
	fs = tcmu_create_glfs_object(path, &hosts);
	if (!fs) {
		tcmu_err("tcmu_create_glfs_object failed\n");
		goto done;
	}
	gfd = glfs_open(fs, hosts->path, ALLOWED_BSOFLAGS);
	if (!gfd) {
		if (asprintf(reason, "glfs_open failed: %m") == -1)
			*reason = NULL;
		result = false;
		goto unref;
	}
	if (glfs_access(fs, hosts->path, R_OK|W_OK) == -1) {
		if (asprintf(reason, "glfs_access file not present, or not writable") == -1)
			*reason = NULL;
		result = false;
		goto unref;
	}
	goto done;
unref:
	gluster_cache_refresh(fs, path);
done:
	if (gfd)
		glfs_close(gfd);
	gluster_free_server(&hosts);
	return result;
}
