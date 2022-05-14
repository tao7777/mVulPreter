static int store_xauthority(void) {
	fs_build_mnt_dir();

	char *src;
	char *dest = RUN_XAUTHORITY_FILE;
	FILE *fp = fopen(dest, "w");
	if (fp) {
		fprintf(fp, "\n");
		SET_PERMS_STREAM(fp, getuid(), getgid(), 0600);
		fclose(fp);
	}
	
	if (asprintf(&src, "%s/.Xauthority", cfg.homedir) == -1)
		errExit("asprintf");
	
	struct stat s;
	if (stat(src, &s) == 0) {
		if (is_link(src)) {
			fprintf(stderr, "Warning: invalid .Xauthority file\n");
 			return 0;
 		}
 
		pid_t child = fork();
		if (child < 0)
			errExit("fork");
		if (child == 0) {
			drop_privs(0);
			int rv = copy_file(src, dest, getuid(), getgid(), 0600);
			if (rv)
				fprintf(stderr, "Warning: cannot transfer .Xauthority in private home directory\n");
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
