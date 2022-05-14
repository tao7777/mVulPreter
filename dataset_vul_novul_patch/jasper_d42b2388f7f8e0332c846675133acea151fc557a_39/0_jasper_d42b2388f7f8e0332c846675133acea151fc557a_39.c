 static int jpg_dec_parseopts(char *optstr, jpg_dec_importopts_t *opts)
 {
 	jas_tvparser_t *tvp;
 
	opts->max_samples = 64 * JAS_MEBI;
 
 	if (!(tvp = jas_tvparser_create(optstr ? optstr : ""))) {
 		return -1;
	}

	while (!jas_tvparser_next(tvp)) {
 		switch (jas_taginfo_nonull(jas_taginfos_lookup(decopts,
 		  jas_tvparser_gettag(tvp)))->id) {
 		case OPT_MAXSIZE:
			opts->max_samples = atoi(jas_tvparser_getval(tvp));
 			break;
 		default:
 			jas_eprintf("warning: ignoring invalid option %s\n",
			  jas_tvparser_gettag(tvp));
			break;
		}
	}

	jas_tvparser_destroy(tvp);

	return 0;
}
