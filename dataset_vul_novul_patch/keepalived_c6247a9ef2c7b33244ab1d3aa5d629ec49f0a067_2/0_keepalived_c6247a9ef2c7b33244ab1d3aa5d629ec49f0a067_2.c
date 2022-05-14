parse_cmdline(int argc, char **argv)
{
	int c;
	bool reopen_log = false;
	int signum;
	struct utsname uname_buf;
	int longindex;
 	int curind;
 	bool bad_option = false;
 	unsigned facility;
	mode_t new_umask_val;
 
 	struct option long_options[] = {
 		{"use-file",		required_argument,	NULL, 'f'},
#if defined _WITH_VRRP_ && defined _WITH_LVS_
		{"vrrp",		no_argument,		NULL, 'P'},
		{"check",		no_argument,		NULL, 'C'},
#endif
#ifdef _WITH_BFD_
		{"no_bfd",		no_argument,		NULL, 'B'},
#endif
		{"all",			no_argument,		NULL,  3 },
		{"log-console",		no_argument,		NULL, 'l'},
		{"log-detail",		no_argument,		NULL, 'D'},
		{"log-facility",	required_argument,	NULL, 'S'},
 		{"log-file",		optional_argument,	NULL, 'g'},
 		{"flush-log-file",	no_argument,		NULL,  2 },
 		{"no-syslog",		no_argument,		NULL, 'G'},
		{"umask",		required_argument,	NULL, 'u'},
 #ifdef _WITH_VRRP_
 		{"release-vips",	no_argument,		NULL, 'X'},
 		{"dont-release-vrrp",	no_argument,		NULL, 'V'},
#endif
#ifdef _WITH_LVS_
		{"dont-release-ipvs",	no_argument,		NULL, 'I'},
#endif
		{"dont-respawn",	no_argument,		NULL, 'R'},
		{"dont-fork",		no_argument,		NULL, 'n'},
		{"dump-conf",		no_argument,		NULL, 'd'},
		{"pid",			required_argument,	NULL, 'p'},
#ifdef _WITH_VRRP_
		{"vrrp_pid",		required_argument,	NULL, 'r'},
#endif
#ifdef _WITH_LVS_
		{"checkers_pid",	required_argument,	NULL, 'c'},
		{"address-monitoring",	no_argument,		NULL, 'a'},
#endif
#ifdef _WITH_BFD_
		{"bfd_pid",		required_argument,	NULL, 'b'},
#endif
#ifdef _WITH_SNMP_
		{"snmp",		no_argument,		NULL, 'x'},
		{"snmp-agent-socket",	required_argument,	NULL, 'A'},
#endif
		{"core-dump",		no_argument,		NULL, 'm'},
		{"core-dump-pattern",	optional_argument,	NULL, 'M'},
#ifdef _MEM_CHECK_LOG_
		{"mem-check-log",	no_argument,		NULL, 'L'},
#endif
#if HAVE_DECL_CLONE_NEWNET
		{"namespace",		required_argument,	NULL, 's'},
#endif
		{"config-id",		required_argument,	NULL, 'i'},
		{"signum",		required_argument,	NULL,  4 },
		{"config-test",		optional_argument,	NULL, 't'},
#ifdef _WITH_PERF_
		{"perf",		optional_argument,	NULL,  5 },
#endif
#ifdef WITH_DEBUG_OPTIONS
		{"debug",		optional_argument,	NULL,  6 },
#endif
		{"version",		no_argument,		NULL, 'v'},
		{"help",		no_argument,		NULL, 'h'},

		{NULL,			0,			NULL,  0 }
	};

	/* Unfortunately, if a short option is used, getopt_long() doesn't change the value
 	 * of longindex, so we need to ensure that before calling getopt_long(), longindex
 	 * is set to a known invalid value */
 	curind = optind;
	while (longindex = -1, (c = getopt_long(argc, argv, ":vhlndu:DRS:f:p:i:mM::g::Gt::"
 #if defined _WITH_VRRP_ && defined _WITH_LVS_
 					    "PC"
 #endif
#ifdef _WITH_VRRP_
					    "r:VX"
#endif
#ifdef _WITH_LVS_
					    "ac:I"
#endif
#ifdef _WITH_BFD_
					    "Bb:"
#endif
#ifdef _WITH_SNMP_
					    "xA:"
#endif
#ifdef _MEM_CHECK_LOG_
					    "L"
#endif
#if HAVE_DECL_CLONE_NEWNET
					    "s:"
#endif
				, long_options, &longindex)) != -1) {

		/* Check for an empty option argument. For example --use-file= returns
		 * a 0 length option, which we don't want */
		if (longindex >= 0 && long_options[longindex].has_arg == required_argument && optarg && !optarg[0]) {
			c = ':';
			optarg = NULL;
		}

		switch (c) {
		case 'v':
			fprintf(stderr, "%s", version_string);
#ifdef GIT_COMMIT
			fprintf(stderr, ", git commit %s", GIT_COMMIT);
#endif
			fprintf(stderr, "\n\n%s\n\n", COPYRIGHT_STRING);
			fprintf(stderr, "Built with kernel headers for Linux %d.%d.%d\n",
						(LINUX_VERSION_CODE >> 16) & 0xff,
						(LINUX_VERSION_CODE >>  8) & 0xff,
						(LINUX_VERSION_CODE      ) & 0xff);
			uname(&uname_buf);
			fprintf(stderr, "Running on %s %s %s\n\n", uname_buf.sysname, uname_buf.release, uname_buf.version);
			fprintf(stderr, "configure options: %s\n\n", KEEPALIVED_CONFIGURE_OPTIONS);
			fprintf(stderr, "Config options: %s\n\n", CONFIGURATION_OPTIONS);
			fprintf(stderr, "System options: %s\n", SYSTEM_OPTIONS);
			exit(0);
			break;
		case 'h':
			usage(argv[0]);
			exit(0);
			break;
		case 'l':
			__set_bit(LOG_CONSOLE_BIT, &debug);
			reopen_log = true;
			break;
		case 'n':
			__set_bit(DONT_FORK_BIT, &debug);
			break;
		case 'd':
			__set_bit(DUMP_CONF_BIT, &debug);
			break;
#ifdef _WITH_VRRP_
		case 'V':
			__set_bit(DONT_RELEASE_VRRP_BIT, &debug);
			break;
#endif
#ifdef _WITH_LVS_
		case 'I':
			__set_bit(DONT_RELEASE_IPVS_BIT, &debug);
			break;
#endif
		case 'D':
			if (__test_bit(LOG_DETAIL_BIT, &debug))
				__set_bit(LOG_EXTRA_DETAIL_BIT, &debug);
			else
				__set_bit(LOG_DETAIL_BIT, &debug);
			break;
		case 'R':
			__set_bit(DONT_RESPAWN_BIT, &debug);
			break;
#ifdef _WITH_VRRP_
		case 'X':
			__set_bit(RELEASE_VIPS_BIT, &debug);
			break;
#endif
		case 'S':
			if (!read_unsigned(optarg, &facility, 0, LOG_FACILITY_MAX, false))
				fprintf(stderr, "Invalid log facility '%s'\n", optarg);
			else {
				log_facility = LOG_FACILITY[facility].facility;
				reopen_log = true;
			}
			break;
		case 'g':
			if (optarg && optarg[0])
				log_file_name = optarg;
			else
				log_file_name = "/tmp/keepalived.log";
			open_log_file(log_file_name, NULL, NULL, NULL);
			break;
		case 'G':
 			__set_bit(NO_SYSLOG_BIT, &debug);
 			reopen_log = true;
 			break;
		case 'u':
			new_umask_val = set_umask(optarg);
			if (umask_cmdline)
				umask_val = new_umask_val;
			break;
 		case 't':
 			__set_bit(CONFIG_TEST_BIT, &debug);
 			__set_bit(DONT_RESPAWN_BIT, &debug);
			__set_bit(DONT_FORK_BIT, &debug);
			__set_bit(NO_SYSLOG_BIT, &debug);
			if (optarg && optarg[0]) {
				int fd = open(optarg, O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
				if (fd == -1) {
					fprintf(stderr, "Unable to open config-test log file %s\n", optarg);
					exit(EXIT_FAILURE);
				}
				dup2(fd, STDERR_FILENO);
				close(fd);
			}
			break;
		case 'f':
			conf_file = optarg;
			break;
		case 2:		/* --flush-log-file */
			set_flush_log_file();
			break;
#if defined _WITH_VRRP_ && defined _WITH_LVS_
		case 'P':
			__clear_bit(DAEMON_CHECKERS, &daemon_mode);
			break;
		case 'C':
			__clear_bit(DAEMON_VRRP, &daemon_mode);
			break;
#endif
#ifdef _WITH_BFD_
		case 'B':
			__clear_bit(DAEMON_BFD, &daemon_mode);
			break;
#endif
		case 'p':
			main_pidfile = optarg;
			break;
#ifdef _WITH_LVS_
		case 'c':
			checkers_pidfile = optarg;
			break;
		case 'a':
			__set_bit(LOG_ADDRESS_CHANGES, &debug);
			break;
#endif
#ifdef _WITH_VRRP_
		case 'r':
			vrrp_pidfile = optarg;
			break;
#endif
#ifdef _WITH_BFD_
		case 'b':
			bfd_pidfile = optarg;
			break;
#endif
#ifdef _WITH_SNMP_
		case 'x':
			snmp = 1;
			break;
		case 'A':
			snmp_socket = optarg;
			break;
#endif
		case 'M':
			set_core_dump_pattern = true;
			if (optarg && optarg[0])
				core_dump_pattern = optarg;
			/* ... falls through ... */
		case 'm':
			create_core_dump = true;
			break;
#ifdef _MEM_CHECK_LOG_
		case 'L':
			__set_bit(MEM_CHECK_LOG_BIT, &debug);
			break;
#endif
#if HAVE_DECL_CLONE_NEWNET
		case 's':
			override_namespace = MALLOC(strlen(optarg) + 1);
			strcpy(override_namespace, optarg);
			break;
#endif
		case 'i':
			FREE_PTR(config_id);
			config_id = MALLOC(strlen(optarg) + 1);
			strcpy(config_id, optarg);
			break;
		case 4:			/* --signum */
			signum = get_signum(optarg);
			if (signum == -1) {
				fprintf(stderr, "Unknown sigfunc %s\n", optarg);
				exit(1);
			}

			printf("%d\n", signum);
			exit(0);
			break;
		case 3:			/* --all */
			__set_bit(RUN_ALL_CHILDREN, &daemon_mode);
#ifdef _WITH_VRRP_
			__set_bit(DAEMON_VRRP, &daemon_mode);
#endif
#ifdef _WITH_LVS_
			__set_bit(DAEMON_CHECKERS, &daemon_mode);
#endif
#ifdef _WITH_BFD_
			__set_bit(DAEMON_BFD, &daemon_mode);
#endif
			break;
#ifdef _WITH_PERF_
		case 5:
			if (optarg && optarg[0]) {
				if (!strcmp(optarg, "run"))
					perf_run = PERF_RUN;
				else if (!strcmp(optarg, "all"))
					perf_run = PERF_ALL;
				else if (!strcmp(optarg, "end"))
					perf_run = PERF_END;
				else
					log_message(LOG_INFO, "Unknown perf start point %s", optarg);
			}
			else
				perf_run = PERF_RUN;

			break;
#endif
#ifdef WITH_DEBUG_OPTIONS
		case 6:
			set_debug_options(optarg && optarg[0] ? optarg : NULL);
			break;
#endif
		case '?':
			if (optopt && argv[curind][1] != '-')
				fprintf(stderr, "Unknown option -%c\n", optopt);
			else
				fprintf(stderr, "Unknown option %s\n", argv[curind]);
			bad_option = true;
			break;
		case ':':
			if (optopt && argv[curind][1] != '-')
				fprintf(stderr, "Missing parameter for option -%c\n", optopt);
			else
				fprintf(stderr, "Missing parameter for option --%s\n", long_options[longindex].name);
			bad_option = true;
			break;
		default:
			exit(1);
			break;
		}
		curind = optind;
	}

	if (optind < argc) {
		printf("Unexpected argument(s): ");
		while (optind < argc)
			printf("%s ", argv[optind++]);
		printf("\n");
	}

	if (bad_option)
		exit(1);

	return reopen_log;
}
