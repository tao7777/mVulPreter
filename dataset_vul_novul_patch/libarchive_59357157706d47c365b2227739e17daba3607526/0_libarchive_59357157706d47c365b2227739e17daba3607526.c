main(int argc, char *argv[])
{
	static char buff[16384];
	struct cpio _cpio; /* Allocated on stack. */
	struct cpio *cpio;
	const char *errmsg;
	int uid, gid;
	int opt;

	cpio = &_cpio;
	memset(cpio, 0, sizeof(*cpio));
	cpio->buff = buff;
	cpio->buff_size = sizeof(buff);

#if defined(HAVE_SIGACTION) && defined(SIGPIPE)
	{ /* Ignore SIGPIPE signals. */
		struct sigaction sa;
		sigemptyset(&sa.sa_mask);
		sa.sa_flags = 0;
		sa.sa_handler = SIG_IGN;
		sigaction(SIGPIPE, &sa, NULL);
	}
#endif

	/* Set lafe_progname before calling lafe_warnc. */
	lafe_setprogname(*argv, "bsdcpio");

#if HAVE_SETLOCALE
	if (setlocale(LC_ALL, "") == NULL)
		lafe_warnc(0, "Failed to set default locale");
#endif

	cpio->uid_override = -1;
	cpio->gid_override = -1;
	cpio->argv = argv;
	cpio->argc = argc;
	cpio->mode = '\0';
	cpio->verbose = 0;
	cpio->compress = '\0';
	cpio->extract_flags = ARCHIVE_EXTRACT_NO_AUTODIR;
 	cpio->extract_flags |= ARCHIVE_EXTRACT_NO_OVERWRITE_NEWER;
 	cpio->extract_flags |= ARCHIVE_EXTRACT_SECURE_SYMLINKS;
 	cpio->extract_flags |= ARCHIVE_EXTRACT_SECURE_NODOTDOT;
	cpio->extract_flags |= ARCHIVE_EXTRACT_SECURE_NOABSOLUTEPATHS;
 	cpio->extract_flags |= ARCHIVE_EXTRACT_PERM;
 	cpio->extract_flags |= ARCHIVE_EXTRACT_FFLAGS;
 	cpio->extract_flags |= ARCHIVE_EXTRACT_ACL;
#if !defined(_WIN32) && !defined(__CYGWIN__)
	if (geteuid() == 0)
		cpio->extract_flags |= ARCHIVE_EXTRACT_OWNER;
#endif
	cpio->bytes_per_block = 512;
	cpio->filename = NULL;

	cpio->matching = archive_match_new();
	if (cpio->matching == NULL)
		lafe_errc(1, 0, "Out of memory");

	while ((opt = cpio_getopt(cpio)) != -1) {
		switch (opt) {
		case '0': /* GNU convention: --null, -0 */
			cpio->option_null = 1;
			break;
		case 'A': /* NetBSD/OpenBSD */
			cpio->option_append = 1;
			break;
		case 'a': /* POSIX 1997 */
			cpio->option_atime_restore = 1;
			break;
		case 'B': /* POSIX 1997 */
			cpio->bytes_per_block = 5120;
			break;
		case OPTION_B64ENCODE:
			cpio->add_filter = opt;
			break;
		case 'C': /* NetBSD/OpenBSD */
			cpio->bytes_per_block = atoi(cpio->argument);
			if (cpio->bytes_per_block <= 0)
				lafe_errc(1, 0, "Invalid blocksize %s", cpio->argument);
			break;
		case 'c': /* POSIX 1997 */
			cpio->format = "odc";
			break;
		case 'd': /* POSIX 1997 */
			cpio->extract_flags &= ~ARCHIVE_EXTRACT_NO_AUTODIR;
			break;
		case 'E': /* NetBSD/OpenBSD */
			if (archive_match_include_pattern_from_file(
			    cpio->matching, cpio->argument,
			    cpio->option_null) != ARCHIVE_OK)
				lafe_errc(1, 0, "Error : %s",
				    archive_error_string(cpio->matching));
			break;
		case 'F': /* NetBSD/OpenBSD/GNU cpio */
			cpio->filename = cpio->argument;
			break;
		case 'f': /* POSIX 1997 */
			if (archive_match_exclude_pattern(cpio->matching,
			    cpio->argument) != ARCHIVE_OK)
				lafe_errc(1, 0, "Error : %s",
				    archive_error_string(cpio->matching));
			break;
		case OPTION_GRZIP:
			cpio->compress = opt;
			break;
		case 'H': /* GNU cpio (also --format) */
			cpio->format = cpio->argument;
			break;
		case 'h':
			long_help();
			break;
		case 'I': /* NetBSD/OpenBSD */
			cpio->filename = cpio->argument;
			break;
		case 'i': /* POSIX 1997 */
			if (cpio->mode != '\0')
				lafe_errc(1, 0,
				    "Cannot use both -i and -%c", cpio->mode);
			cpio->mode = opt;
			break;
		case 'J': /* GNU tar, others */
			cpio->compress = opt;
			break;
		case 'j': /* GNU tar, others */
			cpio->compress = opt;
			break;
 		case OPTION_INSECURE:
 			cpio->extract_flags &= ~ARCHIVE_EXTRACT_SECURE_SYMLINKS;
 			cpio->extract_flags &= ~ARCHIVE_EXTRACT_SECURE_NODOTDOT;
			cpio->extract_flags &= ~ARCHIVE_EXTRACT_SECURE_NOABSOLUTEPATHS;
 			break;
 		case 'L': /* GNU cpio */
 			cpio->option_follow_links = 1;
			break;
		case 'l': /* POSIX 1997 */
			cpio->option_link = 1;
			break;
		case OPTION_LRZIP:
		case OPTION_LZ4:
		case OPTION_LZMA: /* GNU tar, others */
		case OPTION_LZOP: /* GNU tar, others */
			cpio->compress = opt;
			break;
		case 'm': /* POSIX 1997 */
			cpio->extract_flags |= ARCHIVE_EXTRACT_TIME;
			break;
		case 'n': /* GNU cpio */
			cpio->option_numeric_uid_gid = 1;
			break;
		case OPTION_NO_PRESERVE_OWNER: /* GNU cpio */
			cpio->extract_flags &= ~ARCHIVE_EXTRACT_OWNER;
			break;
		case 'O': /* GNU cpio */
			cpio->filename = cpio->argument;
			break;
		case 'o': /* POSIX 1997 */
			if (cpio->mode != '\0')
				lafe_errc(1, 0,
				    "Cannot use both -o and -%c", cpio->mode);
			cpio->mode = opt;
			break;
		case 'p': /* POSIX 1997 */
			if (cpio->mode != '\0')
				lafe_errc(1, 0,
				    "Cannot use both -p and -%c", cpio->mode);
			cpio->mode = opt;
			cpio->extract_flags &= ~ARCHIVE_EXTRACT_SECURE_NODOTDOT;
			break;
		case OPTION_PASSPHRASE:
			cpio->passphrase = cpio->argument;
			break;
		case OPTION_PRESERVE_OWNER:
			cpio->extract_flags |= ARCHIVE_EXTRACT_OWNER;
			break;
		case OPTION_QUIET: /* GNU cpio */
			cpio->quiet = 1;
			break;
		case 'R': /* GNU cpio, also --owner */
			/* TODO: owner_parse should return uname/gname
			 * also; use that to set [ug]name_override. */
			errmsg = owner_parse(cpio->argument, &uid, &gid);
			if (errmsg) {
				lafe_warnc(-1, "%s", errmsg);
				usage();
			}
			if (uid != -1) {
				cpio->uid_override = uid;
				cpio->uname_override = NULL;
			}
			if (gid != -1) {
				cpio->gid_override = gid;
				cpio->gname_override = NULL;
			}
			break;
		case 'r': /* POSIX 1997 */
			cpio->option_rename = 1;
			break;
		case 't': /* POSIX 1997 */
			cpio->option_list = 1;
			break;
		case 'u': /* POSIX 1997 */
			cpio->extract_flags
			    &= ~ARCHIVE_EXTRACT_NO_OVERWRITE_NEWER;
			break;
		case OPTION_UUENCODE:
			cpio->add_filter = opt;
			break;
		case 'v': /* POSIX 1997 */
			cpio->verbose++;
			break;
		case 'V': /* GNU cpio */
			cpio->dot++;
			break;
		case OPTION_VERSION: /* GNU convention */
			version();
			break;
#if 0
	        /*
		 * cpio_getopt() handles -W specially, so it's not
		 * available here.
		 */
		case 'W': /* Obscure, but useful GNU convention. */
			break;
#endif
		case 'y': /* tar convention */
			cpio->compress = opt;
			break;
		case 'Z': /* tar convention */
			cpio->compress = opt;
			break;
		case 'z': /* tar convention */
			cpio->compress = opt;
			break;
		default:
			usage();
		}
	}

	/*
	 * Sanity-check args, error out on nonsensical combinations.
	 */
	/* -t implies -i if no mode was specified. */
	if (cpio->option_list && cpio->mode == '\0')
		cpio->mode = 'i';
	/* -t requires -i */
	if (cpio->option_list && cpio->mode != 'i')
		lafe_errc(1, 0, "Option -t requires -i");
	/* -n requires -it */
	if (cpio->option_numeric_uid_gid && !cpio->option_list)
		lafe_errc(1, 0, "Option -n requires -it");
	/* Can only specify format when writing */
	if (cpio->format != NULL && cpio->mode != 'o')
		lafe_errc(1, 0, "Option --format requires -o");
	/* -l requires -p */
	if (cpio->option_link && cpio->mode != 'p')
		lafe_errc(1, 0, "Option -l requires -p");
	/* -v overrides -V */
	if (cpio->dot && cpio->verbose)
		cpio->dot = 0;
	/* TODO: Flag other nonsensical combinations. */

	switch (cpio->mode) {
	case 'o':
		/* TODO: Implement old binary format in libarchive,
		   use that here. */
		if (cpio->format == NULL)
			cpio->format = "odc"; /* Default format */

		mode_out(cpio);
		break;
	case 'i':
		while (*cpio->argv != NULL) {
			if (archive_match_include_pattern(cpio->matching,
			    *cpio->argv) != ARCHIVE_OK)
				lafe_errc(1, 0, "Error : %s",
				    archive_error_string(cpio->matching));
			--cpio->argc;
			++cpio->argv;
		}
		if (cpio->option_list)
			mode_list(cpio);
		else
			mode_in(cpio);
		break;
	case 'p':
		if (*cpio->argv == NULL || **cpio->argv == '\0')
			lafe_errc(1, 0,
			    "-p mode requires a target directory");
		mode_pass(cpio, *cpio->argv);
		break;
	default:
		lafe_errc(1, 0,
		    "Must specify at least one of -i, -o, or -p");
	}

	archive_match_free(cpio->matching);
	free_cache(cpio->gname_cache);
	free_cache(cpio->uname_cache);
	free(cpio->destdir);
	passphrase_free(cpio->ppbuff);
	return (cpio->return_value);
}
