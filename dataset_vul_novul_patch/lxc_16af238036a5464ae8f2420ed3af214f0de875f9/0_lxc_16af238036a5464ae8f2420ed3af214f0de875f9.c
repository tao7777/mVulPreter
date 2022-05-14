 static int rename_in_ns(int pid, char *oldname, char **newnamep)
 {
	uid_t ruid, suid, euid;
	int fret = -1;
	int fd = -1, ifindex = -1, ofd = -1, ret;
 	bool grab_newname = false;
 
 	ofd = lxc_preserve_ns(getpid(), "net");
 	if (ofd < 0) {
		usernic_error("Failed opening network namespace path for '%d'.", getpid());
		return fret;
 	}
 
 	fd = lxc_preserve_ns(pid, "net");
 	if (fd < 0) {
		usernic_error("Failed opening network namespace path for '%d'.", pid);
		goto do_partial_cleanup;
	}

	ret = getresuid(&ruid, &euid, &suid);
	if (ret < 0) {
		usernic_error("Failed to retrieve real, effective, and saved "
			      "user IDs: %s\n",
			      strerror(errno));
		goto do_partial_cleanup;
	}

	ret = setns(fd, CLONE_NEWNET);
	close(fd);
	fd = -1;
	if (ret < 0) {
		usernic_error("Failed to setns() to the network namespace of "
			      "the container with PID %d: %s.\n",
			      pid, strerror(errno));
		goto do_partial_cleanup;
 	}
 
	ret = setresuid(ruid, ruid, 0);
	if (ret < 0) {
		usernic_error("Failed to drop privilege by setting effective "
			      "user id and real user id to %d, and saved user "
			      "ID to 0: %s.\n",
			      ruid, strerror(errno));
		// COMMENT(brauner): It's ok to jump to do_full_cleanup here
		// since setresuid() will succeed when trying to set real,
		// effective, and saved to values they currently have.
		goto do_full_cleanup;
 	}

 	if (!*newnamep) {
 		grab_newname = true;
 		*newnamep = VETH_DEF_NAME;

		ifindex = if_nametoindex(oldname);
		if (!ifindex) {
			usernic_error("Failed to get netdev index: %s.\n", strerror(errno));
			goto do_full_cleanup;
 		}
 	}

	ret = lxc_netdev_rename_by_name(oldname, *newnamep);
	if (ret < 0) {
		usernic_error("Error %d renaming netdev %s to %s in container.\n", ret, oldname, *newnamep);
		goto do_full_cleanup;
 	}

 	if (grab_newname) {
		char ifname[IFNAMSIZ];
		char *namep = ifname;

 		if (!if_indextoname(ifindex, namep)) {
			usernic_error("Failed to get new netdev name: %s.\n", strerror(errno));
			goto do_full_cleanup;
 		}

 		*newnamep = strdup(namep);
 		if (!*newnamep)
			goto do_full_cleanup;
 	}

	fret = 0;

do_full_cleanup:
	ret = setresuid(ruid, euid, suid);
	if (ret < 0) {
		usernic_error("Failed to restore privilege by setting effective "
			      "user id to %d, real user id to %d, and saved user "
			      "ID to %d: %s.\n",
			      ruid, euid, suid, strerror(errno));
		fret = -1;
		// COMMENT(brauner): setns() should fail if setresuid() doesn't
		// succeed but there's no harm in falling through; keeps the
		// code cleaner.
 	}
 
	ret = setns(ofd, CLONE_NEWNET);
	if (ret < 0) {
		usernic_error("Failed to setns() to original network namespace "
			      "of PID %d: %s.\n",
			      ofd, strerror(errno));
		fret = -1;
	}
 
do_partial_cleanup:
 	if (fd >= 0)
 		close(fd);
	close(ofd);

	return fret;
 }
