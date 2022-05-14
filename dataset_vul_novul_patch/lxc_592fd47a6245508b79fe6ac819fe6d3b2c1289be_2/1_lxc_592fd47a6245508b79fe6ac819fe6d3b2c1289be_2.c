void lxc_execute_bind_init(struct lxc_conf *conf)
{
	int ret;
	char path[PATH_MAX], destpath[PATH_MAX], *p;

	/* If init exists in the container, don't bind mount a static one */
	p = choose_init(conf->rootfs.mount);
	if (p) {
		free(p);
		return;
	}

	ret = snprintf(path, PATH_MAX, SBINDIR "/init.lxc.static");
	if (ret < 0 || ret >= PATH_MAX) {
		WARN("Path name too long searching for lxc.init.static");
		return;
	}

	if (!file_exists(path)) {
		INFO("%s does not exist on host", path);
		return;
	}

	ret = snprintf(destpath, PATH_MAX, "%s%s", conf->rootfs.mount, "/init.lxc.static");
	if (ret < 0 || ret >= PATH_MAX) {
		WARN("Path name too long for container's lxc.init.static");
		return;
	}

	if (!file_exists(destpath)) {
		FILE * pathfile = fopen(destpath, "wb");
		if (!pathfile) {
			SYSERROR("Failed to create mount target '%s'", destpath);
			return;
		}
 		fclose(pathfile);
 	}
 
	ret = mount(path, destpath, "none", MS_BIND, NULL);
 	if (ret < 0)
 		SYSERROR("Failed to bind lxc.init.static into container");
 	INFO("lxc.init.static bound into container at %s", path);
}
