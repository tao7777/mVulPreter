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

		copy_file_as_user(src, dest, getuid(), getgid(), 0600);
		fs_logger2("clone", dest);
		return 1; // file copied
	}
	
	return 0;
}
