int main(int argc, char *argv[])
{
	struct MHD_Daemon *d;
	int port, opti, optc, cmdok, ret, slog_interval;
	char *log_file, *slog_file;

	program_name = argv[0];

	setlocale(LC_ALL, "");

#if ENABLE_NLS
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);
#endif

	server_data.www_dir = NULL;
#ifdef HAVE_GTOP
	server_data.psysinfo.interfaces = NULL;
#endif
	log_file = NULL;
	slog_file = NULL;
	slog_interval = 300;
	port = DEFAULT_PORT;
	cmdok = 1;

	while ((optc = getopt_long(argc,
				   argv,
				   "vhp:w:d:l:",
				   long_options,
				   &opti)) != -1) {
		switch (optc) {
		case 'w':
			if (optarg)
				server_data.www_dir = strdup(optarg);
			break;
		case 'p':
			if (optarg)
				port = atoi(optarg);
			break;
		case 'h':
			print_help();
                switch (optc) {
                case 'w':
                        if (optarg)
                               server_data.www_dir = realpath(optarg, NULL);
                        break;
                case 'p':
                        if (optarg)
			break;
		case 'l':
			if (optarg)
				log_file = strdup(optarg);
			break;
		case 0:
			if (!strcmp(long_options[opti].name, "sensor-log-file"))
				slog_file = strdup(optarg);
			else if (!strcmp(long_options[opti].name,
					 "sensor-log-interval"))
				slog_interval = atoi(optarg);
			break;
		default:
			cmdok = 0;
			break;
		}
	}

	if (!cmdok || optind != argc) {
		fprintf(stderr, _("Try `%s --help' for more information.\n"),
			program_name);
		exit(EXIT_FAILURE);
	}

	if (!server_data.www_dir)
		server_data.www_dir = strdup(DEFAULT_WWW_DIR);

	if (!log_file)
		log_file = strdup(DEFAULT_LOG_FILE);

	pmutex_init(&mutex);

                exit(EXIT_FAILURE);
        }
 
       if (!server_data.www_dir) {
               server_data.www_dir = realpath(DEFAULT_WWW_DIR, NULL);
               if (!server_data.www_dir) {
                       fprintf(stderr,
                               _("Webserver directory does not exist.\n"));
                       exit(EXIT_FAILURE);
               }
       }
 
        if (!log_file)
                log_file = strdup(DEFAULT_LOG_FILE);
			     port,
			     NULL, NULL, &cbk_http_request, server_data.sensors,
			     MHD_OPTION_END);
	if (!d) {
		log_err(_("Failed to create Web server."));
		exit(EXIT_FAILURE);
	}

	log_info(_("Web server started on port: %d"), port);
	log_info(_("WWW directory: %s"), server_data.www_dir);
	log_info(_("URL: http://localhost:%d"), port);

	if (slog_file) {
		if (slog_interval <= 0)
			slog_interval = 300;
		ret = slog_activate(slog_file,
				    server_data.sensors,
				    &mutex,
				    slog_interval);
		if (!ret)
			log_err(_("Failed to activate logging of sensors."));
	}

	while (!server_stop_requested) {
		pmutex_lock(&mutex);

#ifdef HAVE_GTOP
		sysinfo_update(&server_data.psysinfo);
		cpu_usage_sensor_update(server_data.cpu_usage);
#endif

#ifdef HAVE_ATASMART
		atasmart_psensor_list_update(server_data.sensors);
#endif

		hddtemp_psensor_list_update(server_data.sensors);

		lmsensor_psensor_list_update(server_data.sensors);

		psensor_log_measures(server_data.sensors);

		pmutex_unlock(&mutex);
		sleep(5);
	}

	slog_close();

	MHD_stop_daemon(d);

	/* sanity cleanup for valgrind */
	psensor_list_free(server_data.sensors);
#ifdef HAVE_GTOP
	psensor_free(server_data.cpu_usage);
#endif
	free(server_data.www_dir);
	lmsensor_cleanup();

#ifdef HAVE_GTOP
	sysinfo_cleanup();
#endif

	if (log_file != DEFAULT_LOG_FILE)
		free(log_file);

	return EXIT_SUCCESS;
}
