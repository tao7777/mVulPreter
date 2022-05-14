void fslib_copy_libs(const char *full_path) {
	assert(full_path);
	if (arg_debug || arg_debug_private_lib)
		printf("    fslib_copy_libs %s\n", full_path);

	if (access(full_path, R_OK)) {
		if (arg_debug || arg_debug_private_lib)
			printf("cannot find %s for private-lib, skipping...\n", full_path);
		return;
	}

	unlink(RUN_LIB_FILE);			  // in case is there
	create_empty_file_as_root(RUN_LIB_FILE, 0644);
	if (chown(RUN_LIB_FILE, getuid(), getgid()))
		errExit("chown");

	if (arg_debug || arg_debug_private_lib)
		printf("    running fldd %s\n", full_path);
	sbox_run(SBOX_USER | SBOX_SECCOMP | SBOX_CAPS_NONE, 3, PATH_FLDD, full_path, RUN_LIB_FILE);

	FILE *fp = fopen(RUN_LIB_FILE, "r");
	if (!fp)
		errExit("fopen");

	char buf[MAXBUF];
	while (fgets(buf, MAXBUF, fp)) {
		char *ptr = strchr(buf, '\n');
		if (ptr)
			*ptr = '\0';
 		fslib_duplicate(buf);
 	}
 	fclose(fp);
	unlink(RUN_LIB_FILE);
 }
