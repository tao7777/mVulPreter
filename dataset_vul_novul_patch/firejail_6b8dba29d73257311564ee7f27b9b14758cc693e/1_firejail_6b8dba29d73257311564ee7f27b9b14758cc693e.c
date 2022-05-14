static void detect_allow_debuggers(int argc, char **argv) {
	int i;
	
 	for (i = 1; i < argc; i++) {
 		if (strcmp(argv[i], "--allow-debuggers") == 0) {
 			arg_allow_debuggers = 1;
 			break;
 		}
		
		if (strcmp(argv[i], "--") == 0)
			break;
		if (strncmp(argv[i], "--", 2) != 0)
			break;
	}
}
