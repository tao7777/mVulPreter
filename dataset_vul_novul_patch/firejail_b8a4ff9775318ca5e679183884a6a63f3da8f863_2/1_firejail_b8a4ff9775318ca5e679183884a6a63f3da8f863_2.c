static int store_asoundrc(void) {
	fs_build_mnt_dir();

	char *src;
	char *dest = RUN_ASOUNDRC_FILE;
	FILE *fp = fopen(dest, "w");
	if (fp) {
		fprintf(fp, "\n");
		SET_PERMS_STREAM(fp, getuid(), getgid(), 0644);
		fclose(fp);
	}
	
	if (asprintf(&src, "%s/.asoundrc", cfg.homedir) == -1)
		errExit("asprintf");
	
	struct stat s;
	if (stat(src, &s) == 0) {
		if (is_link(src)) {
			/* coverity[toctou] */
			char* rp = realpath(src, NULL);
			if (!rp) {
				fprintf(stderr, "Error: Cannot access %s\n", src);
				exit(1);
			}
			if (strncmp(rp, cfg.homedir, strlen(cfg.homedir)) != 0) {
				fprintf(stderr, "Error: .asoundrc is a symbolic link pointing to a file outside home directory\n");
				exit(1);
			}
 			free(rp);
 		}
 
		pid_t child = fork();
		if (child < 0)
			errExit("fork");
		if (child == 0) {
			drop_privs(0);
			int rv = copy_file(src, dest, getuid(), getgid(), 0644);
			if (rv)
				fprintf(stderr, "Warning: cannot transfer .asoundrc in private home directory\n");
			else {
				fs_logger2("clone", dest);
			}
			_exit(0);
		}
		waitpid(child, NULL, 0);
 		return 1; // file copied
 	}
 	
	return 0;
}
