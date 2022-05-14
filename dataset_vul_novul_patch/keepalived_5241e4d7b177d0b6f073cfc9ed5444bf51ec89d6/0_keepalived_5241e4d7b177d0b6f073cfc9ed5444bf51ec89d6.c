set_umask(const char *optarg)
{
	long umask_long;
	mode_t umask_val;
	char *endptr;

	umask_long = strtoll(optarg, &endptr, 0);
 
 	if (*endptr || umask_long < 0 || umask_long & ~0777L) {
 		fprintf(stderr, "Invalid --umask option %s", optarg);
		return 0;
 	}
 
 	umask_val = umask_long & 0777;
	umask(umask_val);

	umask_cmdline = true;

	return umask_val;
}
