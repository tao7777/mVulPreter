int main(int argc, char ** argv)
{
	int c;
	unsigned long flags = MS_MANDLOCK;
	char * orgoptions = NULL;
	char * share_name = NULL;
	const char * ipaddr = NULL;
	char * uuid = NULL;
	char * mountpoint = NULL;
	char * options = NULL;
	char * optionstail;
	char * resolved_path = NULL;
	char * temp;
	char * dev_name;
	int rc = 0;
	int rsize = 0;
	int wsize = 0;
	int nomtab = 0;
	int uid = 0;
	int gid = 0;
	int optlen = 0;
	int orgoptlen = 0;
	size_t options_size = 0;
	size_t current_len;
	int retry = 0; /* set when we have to retry mount with uppercase */
	struct addrinfo *addrhead = NULL, *addr;
	struct utsname sysinfo;
	struct mntent mountent;
	struct sockaddr_in *addr4;
	struct sockaddr_in6 *addr6;
	FILE * pmntfile;

	/* setlocale(LC_ALL, "");
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE); */

	if(argc && argv)
		thisprogram = argv[0];
	else
		mount_cifs_usage(stderr);

	if(thisprogram == NULL)
		thisprogram = "mount.cifs";

	uname(&sysinfo);
	/* BB add workstation name and domain and pass down */

/* #ifdef _GNU_SOURCE
	fprintf(stderr, " node: %s machine: %s sysname %s domain %s\n", sysinfo.nodename,sysinfo.machine,sysinfo.sysname,sysinfo.domainname);
#endif */
	if(argc > 2) {
		dev_name = argv[1];
		share_name = strndup(argv[1], MAX_UNC_LEN);
		if (share_name == NULL) {
			fprintf(stderr, "%s: %s", argv[0], strerror(ENOMEM));
			exit(EX_SYSERR);
		}
		mountpoint = argv[2];
	} else if (argc == 2) {
		if ((strcmp(argv[1], "-V") == 0) ||
		    (strcmp(argv[1], "--version") == 0))
		{
			print_cifs_mount_version();
			exit(0);
		}

		if ((strcmp(argv[1], "-h") == 0) ||
		    (strcmp(argv[1], "-?") == 0) ||
		    (strcmp(argv[1], "--help") == 0))
			mount_cifs_usage(stdout);

		mount_cifs_usage(stderr);
	} else {
		mount_cifs_usage(stderr);
	}


	/* add sharename in opts string as unc= parm */
	while ((c = getopt_long (argc, argv, "afFhilL:no:O:rsSU:vVwt:",
			 longopts, NULL)) != -1) {
		switch (c) {
/* No code to do the following  options yet */
/*	case 'l':
		list_with_volumelabel = 1;
		break;
	case 'L':
		volumelabel = optarg;
		break; */
/*	case 'a':	       
		++mount_all;
		break; */

		case '?':
		case 'h':	 /* help */
			mount_cifs_usage(stdout);
		case 'n':
			++nomtab;
			break;
		case 'b':
#ifdef MS_BIND
			flags |= MS_BIND;
#else
			fprintf(stderr,
				"option 'b' (MS_BIND) not supported\n");
#endif
			break;
		case 'm':
#ifdef MS_MOVE		      
			flags |= MS_MOVE;
#else
			fprintf(stderr,
				"option 'm' (MS_MOVE) not supported\n");
#endif
			break;
		case 'o':
			orgoptions = strdup(optarg);
		    break;
		case 'r':  /* mount readonly */
			flags |= MS_RDONLY;
			break;
		case 'U':
			uuid = optarg;
			break;
		case 'v':
			++verboseflag;
			break;
		case 'V':
			print_cifs_mount_version();
			exit (0);
		case 'w':
			flags &= ~MS_RDONLY;
			break;
		case 'R':
			rsize = atoi(optarg) ;
			break;
		case 'W':
			wsize = atoi(optarg);
			break;
		case '1':
			if (isdigit(*optarg)) {
				char *ep;

				uid = strtoul(optarg, &ep, 10);
				if (*ep) {
					fprintf(stderr, "bad uid value \"%s\"\n", optarg);
					exit(EX_USAGE);
				}
			} else {
				struct passwd *pw;

				if (!(pw = getpwnam(optarg))) {
					fprintf(stderr, "bad user name \"%s\"\n", optarg);
					exit(EX_USAGE);
				}
				uid = pw->pw_uid;
				endpwent();
			}
			break;
		case '2':
			if (isdigit(*optarg)) {
				char *ep;

				gid = strtoul(optarg, &ep, 10);
				if (*ep) {
					fprintf(stderr, "bad gid value \"%s\"\n", optarg);
					exit(EX_USAGE);
				}
			} else {
				struct group *gr;

				if (!(gr = getgrnam(optarg))) {
					fprintf(stderr, "bad user name \"%s\"\n", optarg);
					exit(EX_USAGE);
				}
				gid = gr->gr_gid;
				endpwent();
			}
			break;
		case 'u':
			got_user = 1;
			user_name = optarg;
			break;
		case 'd':
			domain_name = optarg; /* BB fix this - currently ignored */
			got_domain = 1;
			break;
		case 'p':
			if(mountpassword == NULL)
				mountpassword = (char *)calloc(MOUNT_PASSWD_SIZE+1,1);
			if(mountpassword) {
				got_password = 1;
				strlcpy(mountpassword,optarg,MOUNT_PASSWD_SIZE+1);
			}
			break;
		case 'S':
			get_password_from_file(0 /* stdin */,NULL);
			break;
		case 't':
			break;
		case 'f':
			++fakemnt;
			break;
		default:
			fprintf(stderr, "unknown mount option %c\n",c);
			mount_cifs_usage(stderr);
		}
	}

	if((argc < 3) || (dev_name == NULL) || (mountpoint == NULL)) {
		mount_cifs_usage(stderr);
        }
 
        /* make sure mountpoint is legit */
       rc = chdir(mountpoint);
       if (rc) {
               fprintf(stderr, "Couldn't chdir to %s: %s\n", mountpoint,
                               strerror(errno));
               rc = EX_USAGE;
               goto mount_exit;
       }

        rc = check_mountpoint(thisprogram, mountpoint);
        if (rc)
                goto mount_exit;
		/* enable any default user mount flags */
		flags |= CIFS_SETUID_FLAGS;
	}
