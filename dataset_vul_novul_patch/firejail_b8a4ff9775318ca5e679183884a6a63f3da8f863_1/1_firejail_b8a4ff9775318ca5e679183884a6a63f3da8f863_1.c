static void skel(const char *homedir, uid_t u, gid_t g) {
	char *fname;

	if (!arg_shell_none && (strcmp(cfg.shell,"/usr/bin/zsh") == 0 || strcmp(cfg.shell,"/bin/zsh") == 0)) {
		if (asprintf(&fname, "%s/.zshrc", homedir) == -1)
			errExit("asprintf");
		struct stat s;
 		if (stat(fname, &s) == 0)
 			return;
 		if (stat("/etc/skel/.zshrc", &s) == 0) {
			if (copy_file("/etc/skel/.zshrc", fname, u, g, 0644) == 0) {
				fs_logger("clone /etc/skel/.zshrc");
			}
 		}
		else { // 
			FILE *fp = fopen(fname, "w");
			if (fp) {
				fprintf(fp, "\n");
				SET_PERMS_STREAM(fp, u, g, S_IRUSR | S_IWUSR);
				fclose(fp);
				fs_logger2("touch", fname);
			}
 		}
 		free(fname);
 	}
	else if (!arg_shell_none && strcmp(cfg.shell,"/bin/csh") == 0) {
		if (asprintf(&fname, "%s/.cshrc", homedir) == -1)
			errExit("asprintf");
		struct stat s;
 		if (stat(fname, &s) == 0)
 			return;
 		if (stat("/etc/skel/.cshrc", &s) == 0) {
			if (copy_file("/etc/skel/.cshrc", fname, u, g, 0644) == 0) {
				fs_logger("clone /etc/skel/.cshrc");
			}
 		}
		else { // 
			/* coverity[toctou] */
			FILE *fp = fopen(fname, "w");
			if (fp) {
				fprintf(fp, "\n");
				SET_PERMS_STREAM(fp, u, g, S_IRUSR | S_IWUSR);
				fclose(fp);
				fs_logger2("touch", fname);
			}
 		}
 		free(fname);
 	}
	else {
		if (asprintf(&fname, "%s/.bashrc", homedir) == -1)
			errExit("asprintf");
		struct stat s;
 		if (stat(fname, &s) == 0) 
 			return;
 		if (stat("/etc/skel/.bashrc", &s) == 0) {
			if (copy_file("/etc/skel/.bashrc", fname, u, g, 0644) == 0) {
				fs_logger("clone /etc/skel/.bashrc");
			}
 		}
 		free(fname);
 	}
}
