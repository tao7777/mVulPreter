static void detect_allow_debuggers(int argc, char **argv) {
	int i;
	
 	for (i = 1; i < argc; i++) {
 		if (strcmp(argv[i], "--allow-debuggers") == 0) {
			// check kernel version
			struct utsname u;
			int rv = uname(&u);
			if (rv != 0)
				errExit("uname");
			int major;
			int minor;
			if (2 != sscanf(u.release, "%d.%d", &major, &minor)) {
				fprintf(stderr, "Error: cannot extract Linux kernel version: %s\n", u.version);
				exit(1);
			}
			if (major < 4 || (major == 4 && minor < 8)) {
				fprintf(stderr, "Error: --allow-debuggers is disabled on Linux kernels prior to 4.8. "
					"A bug in ptrace call allows a full bypass of the seccomp filter. "
					"Your current kernel version is %d.%d.\n", major, minor);
				exit(1);
			}
			
 			arg_allow_debuggers = 1;
 			break;
 		}
		
		if (strcmp(argv[i], "--") == 0)
			break;
		if (strncmp(argv[i], "--", 2) != 0)
			break;
	}
}
