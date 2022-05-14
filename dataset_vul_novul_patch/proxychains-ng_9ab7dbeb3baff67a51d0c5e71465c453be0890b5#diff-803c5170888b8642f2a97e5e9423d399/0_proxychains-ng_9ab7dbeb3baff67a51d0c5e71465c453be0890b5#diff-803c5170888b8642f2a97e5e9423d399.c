static void set_own_dir(const char *argv0) {
	size_t l = strlen(argv0);
 	while(l && argv0[l - 1] != '/')
 		l--;
 	if(l == 0)
#ifdef SUPER_SECURE
		memcpy(own_dir, "/dev/null/", 2);
#else
 		memcpy(own_dir, ".", 2);
#endif
 	else {
 		memcpy(own_dir, argv0, l - 1);
 		own_dir[l] = 0;
	}
}
