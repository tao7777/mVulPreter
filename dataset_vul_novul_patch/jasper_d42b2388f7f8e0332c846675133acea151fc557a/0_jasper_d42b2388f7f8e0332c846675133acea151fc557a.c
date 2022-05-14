int main(int argc, char **argv)
{
	int fmtid;
	int id;
	char *infile;
	jas_stream_t *instream;
	jas_image_t *image;
	int width;
	int height;
	int depth;
	int numcmpts;
	int verbose;
 	char *fmtname;
 	int debug;
 	size_t max_mem;
	size_t max_samples;
	char optstr[32];
 
 	if (jas_init()) {
 		abort();
 	}
 
 	cmdname = argv[0];
 
	max_samples = 64 * JAS_MEBI;
 	infile = 0;
 	verbose = 0;
 	debug = 0;
#if defined(JAS_DEFAULT_MAX_MEM_USAGE)
	max_mem = JAS_DEFAULT_MAX_MEM_USAGE;
#endif

	/* Parse the command line options. */
	while ((id = jas_getopt(argc, argv, opts)) >= 0) {
		switch (id) {
		case OPT_VERBOSE:
			verbose = 1;
			break;
		case OPT_VERSION:
			printf("%s\n", JAS_VERSION);
			exit(EXIT_SUCCESS);
			break;
		case OPT_DEBUG:
			debug = atoi(jas_optarg);
			break;
 		case OPT_INFILE:
 			infile = jas_optarg;
 			break;
		case OPT_MAXSAMPLES:
			max_samples = strtoull(jas_optarg, 0, 10);
			break;
 		case OPT_MAXMEM:
 			max_mem = strtoull(jas_optarg, 0, 10);
 			break;
		case OPT_HELP:
		default:
			usage();
			break;
		}
	}

	jas_setdbglevel(debug);
#if defined(JAS_DEFAULT_MAX_MEM_USAGE)
	jas_set_max_mem_usage(max_mem);
#endif

	/* Open the image file. */
	if (infile) {
		/* The image is to be read from a file. */
		if (!(instream = jas_stream_fopen(infile, "rb"))) {
			fprintf(stderr, "cannot open input image file %s\n", infile);
			exit(EXIT_FAILURE);
		}
	} else {
		/* The image is to be read from standard input. */
		if (!(instream = jas_stream_fdopen(0, "rb"))) {
			fprintf(stderr, "cannot open standard input\n");
			exit(EXIT_FAILURE);
		}
	}

	if ((fmtid = jas_image_getfmt(instream)) < 0) {
 		fprintf(stderr, "unknown image format\n");
 	}
 
	snprintf(optstr, sizeof(optstr), "max_samples=%-zu", max_samples);

 	/* Decode the image. */
	if (!(image = jas_image_decode(instream, fmtid, optstr))) {
 		jas_stream_close(instream);
 		fprintf(stderr, "cannot load image\n");
 		return EXIT_FAILURE;
	}

 	/* Close the image file. */
 	jas_stream_close(instream);
 
	if (!(fmtname = jas_image_fmttostr(fmtid))) {
		jas_eprintf("format name lookup failed\n");
		return EXIT_FAILURE;
	}

 	if (!(numcmpts = jas_image_numcmpts(image))) {
 		fprintf(stderr, "warning: image has no components\n");
 	}
	if (numcmpts) {
		width = jas_image_cmptwidth(image, 0);
		height = jas_image_cmptheight(image, 0);
		depth = jas_image_cmptprec(image, 0);
	} else {
		width = 0;
 		height = 0;
 		depth = 0;
 	}
	printf("%s %d %d %d %d %ld\n", fmtname, numcmpts, width, height, depth,
	  JAS_CAST(long, jas_image_rawsize(image)));
 
 	jas_image_destroy(image);
 	jas_image_clearfmts();

	return EXIT_SUCCESS;
}
