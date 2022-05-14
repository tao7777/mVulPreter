int mp4client_main(int argc, char **argv)
{
	char c;
	const char *str;
	int ret_val = 0;
	u32 i, times[100], nb_times, dump_mode;
	u32 simulation_time_in_ms = 0;
	u32 initial_service_id = 0;
	Bool auto_exit = GF_FALSE;
	Bool logs_set = GF_FALSE;
	Bool start_fs = GF_FALSE;
	Bool use_rtix = GF_FALSE;
	Bool pause_at_first = GF_FALSE;
	Bool no_cfg_save = GF_FALSE;
	Bool is_cfg_only = GF_FALSE;

	Double play_from = 0;
#ifdef GPAC_MEMORY_TRACKING
    GF_MemTrackerType mem_track = GF_MemTrackerNone;
#endif
	Double fps = GF_IMPORT_DEFAULT_FPS;
	Bool fill_ar, visible, do_uncache, has_command;
	char *url_arg, *out_arg, *the_cfg, *rti_file, *views, *mosaic;
	FILE *logfile = NULL;
	Float scale = 1;
#ifndef WIN32
	dlopen(NULL, RTLD_NOW|RTLD_GLOBAL);
#endif

	/*by default use current dir*/
	strcpy(the_url, ".");

	memset(&user, 0, sizeof(GF_User));

	dump_mode = DUMP_NONE;
	fill_ar = visible = do_uncache = has_command = GF_FALSE;
	url_arg = out_arg = the_cfg = rti_file = views = mosaic = NULL;
	nb_times = 0;
	times[0] = 0;

	/*first locate config file if specified*/
	for (i=1; i<(u32) argc; i++) {
		char *arg = argv[i];
		if (!strcmp(arg, "-c") || !strcmp(arg, "-cfg")) {
			the_cfg = argv[i+1];
			i++;
		}
		else if (!strcmp(arg, "-mem-track") || !strcmp(arg, "-mem-track-stack")) {
#ifdef GPAC_MEMORY_TRACKING
            mem_track = !strcmp(arg, "-mem-track-stack") ? GF_MemTrackerBackTrace : GF_MemTrackerSimple;
#else
			fprintf(stderr, "WARNING - GPAC not compiled with Memory Tracker - ignoring \"%s\"\n", arg);
#endif
		} else if (!strcmp(arg, "-gui")) {
			gui_mode = 1;
		} else if (!strcmp(arg, "-guid")) {
			gui_mode = 2;
		} else if (!strcmp(arg, "-h") || !strcmp(arg, "-help")) {
			PrintUsage();
			return 0;
		}
	}

#ifdef GPAC_MEMORY_TRACKING
	gf_sys_init(mem_track);
#else
	gf_sys_init(GF_MemTrackerNone);
#endif
	gf_sys_set_args(argc, (const char **) argv);

	cfg_file = gf_cfg_init(the_cfg, NULL);
	if (!cfg_file) {
		fprintf(stderr, "Error: Configuration File not found\n");
		return 1;
	}
	/*if logs are specified, use them*/
	if (gf_log_set_tools_levels( gf_cfg_get_key(cfg_file, "General", "Logs") ) != GF_OK) {
		return 1;
	}

	if( gf_cfg_get_key(cfg_file, "General", "Logs") != NULL ) {
		logs_set = GF_TRUE;
	}

	if (!gui_mode) {
		str = gf_cfg_get_key(cfg_file, "General", "ForceGUI");
		if (str && !strcmp(str, "yes")) gui_mode = 1;
	}

	for (i=1; i<(u32) argc; i++) {
		char *arg = argv[i];

		if (!strcmp(arg, "-rti")) {
			rti_file = argv[i+1];
			i++;
		} else if (!strcmp(arg, "-rtix")) {
			rti_file = argv[i+1];
			i++;
			use_rtix = GF_TRUE;
		} else if (!stricmp(arg, "-size")) {
			/*usage of %ud breaks sscanf on MSVC*/
			if (sscanf(argv[i+1], "%dx%d", &forced_width, &forced_height) != 2) {
				forced_width = forced_height = 0;
			}
			i++;
		} else if (!strcmp(arg, "-quiet")) {
			be_quiet = 1;
		} else if (!strcmp(arg, "-strict-error")) {
			gf_log_set_strict_error(1);
		} else if (!strcmp(arg, "-log-file") || !strcmp(arg, "-lf")) {
			logfile = gf_fopen(argv[i+1], "wt");
			gf_log_set_callback(logfile, on_gpac_log);
			i++;
		} else if (!strcmp(arg, "-logs") ) {
			if (gf_log_set_tools_levels(argv[i+1]) != GF_OK) {
				return 1;
			}
			logs_set = GF_TRUE;
			i++;
		} else if (!strcmp(arg, "-log-clock") || !strcmp(arg, "-lc")) {
			log_time_start = 1;
		} else if (!strcmp(arg, "-log-utc") || !strcmp(arg, "-lu")) {
			log_utc_time = 1;
		}
#if defined(__DARWIN__) || defined(__APPLE__)
		else if (!strcmp(arg, "-thread")) threading_flags = 0;
#else
		else if (!strcmp(arg, "-no-thread")) threading_flags = GF_TERM_NO_DECODER_THREAD | GF_TERM_NO_COMPOSITOR_THREAD | GF_TERM_WINDOW_NO_THREAD;
#endif
		else if (!strcmp(arg, "-no-cthread") || !strcmp(arg, "-no-compositor-thread")) threading_flags |= GF_TERM_NO_COMPOSITOR_THREAD;
		else if (!strcmp(arg, "-no-audio")) no_audio = 1;
		else if (!strcmp(arg, "-no-regulation")) no_regulation = 1;
		else if (!strcmp(arg, "-fs")) start_fs = 1;

		else if (!strcmp(arg, "-opt")) {
			set_cfg_option(argv[i+1]);
			i++;
		} else if (!strcmp(arg, "-conf")) {
			set_cfg_option(argv[i+1]);
			is_cfg_only=GF_TRUE;
			i++;
		}
		else if (!strcmp(arg, "-ifce")) {
			gf_cfg_set_key(cfg_file, "Network", "DefaultMCastInterface", argv[i+1]);
			i++;
		}
		else if (!stricmp(arg, "-help")) {
			PrintUsage();
			return 1;
		}
		else if (!stricmp(arg, "-noprog")) {
			no_prog=1;
			gf_set_progress_callback(NULL, progress_quiet);
		}
		else if (!stricmp(arg, "-no-save") || !stricmp(arg, "--no-save") /*old versions used --n-save ...*/) {
			no_cfg_save=1;
		}
		else if (!stricmp(arg, "-ntp-shift")) {
			s32 shift = atoi(argv[i+1]);
			i++;
			gf_net_set_ntp_shift(shift);
		}
		else if (!stricmp(arg, "-run-for")) {
			simulation_time_in_ms = atoi(argv[i+1]) * 1000;
			if (!simulation_time_in_ms)
				simulation_time_in_ms = 1; /*1ms*/
			i++;
		}

		else if (!strcmp(arg, "-out")) {
			out_arg = argv[i+1];
			i++;
		}
		else if (!stricmp(arg, "-fps")) {
			fps = atof(argv[i+1]);
			i++;
		} else if (!strcmp(arg, "-avi") || !strcmp(arg, "-sha")) {
			dump_mode &= 0xFFFF0000;

			if (!strcmp(arg, "-sha")) dump_mode |= DUMP_SHA1;
			else dump_mode |= DUMP_AVI;

			if ((url_arg || (i+2<(u32)argc)) && get_time_list(argv[i+1], times, &nb_times)) {
				if (!strcmp(arg, "-avi") && (nb_times!=2) ) {
					fprintf(stderr, "Only one time arg found for -avi - check usage\n");
					return 1;
				}
				i++;
			}
		} else if (!strcmp(arg, "-rgbds")) { /*get dump in rgbds pixel format*/
				dump_mode |= DUMP_RGB_DEPTH_SHAPE;
		} else if (!strcmp(arg, "-rgbd")) { /*get dump in rgbd pixel format*/
				dump_mode |= DUMP_RGB_DEPTH;
		} else if (!strcmp(arg, "-depth")) {
				dump_mode |= DUMP_DEPTH_ONLY;
		} else if (!strcmp(arg, "-bmp")) {
			dump_mode &= 0xFFFF0000;
			dump_mode |= DUMP_BMP;
			if ((url_arg || (i+2<(u32)argc)) && get_time_list(argv[i+1], times, &nb_times)) i++;
		} else if (!strcmp(arg, "-png")) {
			dump_mode &= 0xFFFF0000;
			dump_mode |= DUMP_PNG;
			if ((url_arg || (i+2<(u32)argc)) && get_time_list(argv[i+1], times, &nb_times)) i++;
		} else if (!strcmp(arg, "-raw")) {
			dump_mode &= 0xFFFF0000;
			dump_mode |= DUMP_RAW;
			if ((url_arg || (i+2<(u32)argc)) && get_time_list(argv[i+1], times, &nb_times)) i++;
		} else if (!stricmp(arg, "-scale")) {
			sscanf(argv[i+1], "%f", &scale);
			i++;
		}
		else if (!strcmp(arg, "-c") || !strcmp(arg, "-cfg")) {
			/* already parsed */
			i++;
		}

		/*arguments only used in non-gui mode*/
		if (!gui_mode) {
			if (arg[0] != '-') {
				if (url_arg) {
					fprintf(stderr, "Several input URLs provided (\"%s\", \"%s\"). Check your command-line.\n", url_arg, arg);
					return 1;
				}
				url_arg = arg;
			}
			else if (!strcmp(arg, "-loop")) loop_at_end = 1;
			else if (!strcmp(arg, "-bench")) bench_mode = 1;
			else if (!strcmp(arg, "-vbench")) bench_mode = 2;
			else if (!strcmp(arg, "-sbench")) bench_mode = 3;
			else if (!strcmp(arg, "-no-addon")) enable_add_ons = GF_FALSE;

			else if (!strcmp(arg, "-pause")) pause_at_first = 1;
			else if (!strcmp(arg, "-play-from")) {
				play_from = atof((const char *) argv[i+1]);
				i++;
			}
			else if (!strcmp(arg, "-speed")) {
				playback_speed = FLT2FIX( atof((const char *) argv[i+1]) );
				if (playback_speed <= 0) playback_speed = FIX_ONE;
				i++;
			}
			else if (!strcmp(arg, "-no-wnd")) user.init_flags |= GF_TERM_WINDOWLESS;
			else if (!strcmp(arg, "-no-back")) user.init_flags |= GF_TERM_WINDOW_TRANSPARENT;
			else if (!strcmp(arg, "-align")) {
				if (argv[i+1][0]=='m') align_mode = 1;
				else if (argv[i+1][0]=='b') align_mode = 2;
				align_mode <<= 8;
				if (argv[i+1][1]=='m') align_mode |= 1;
				else if (argv[i+1][1]=='r') align_mode |= 2;
				i++;
			} else if (!strcmp(arg, "-fill")) {
				fill_ar = GF_TRUE;
			} else if (!strcmp(arg, "-show")) {
				visible = 1;
			} else if (!strcmp(arg, "-uncache")) {
				do_uncache = GF_TRUE;
			}
			else if (!strcmp(arg, "-exit")) auto_exit = GF_TRUE;
			else if (!stricmp(arg, "-views")) {
				views = argv[i+1];
				i++;
			}
			else if (!stricmp(arg, "-mosaic")) {
				mosaic = argv[i+1];
				i++;
			}
			else if (!stricmp(arg, "-com")) {
				has_command = GF_TRUE;
				i++;
			}
			else if (!stricmp(arg, "-service")) {
				initial_service_id = atoi(argv[i+1]);
				i++;
			}
		}
	}
	if (is_cfg_only) {
		gf_cfg_del(cfg_file);
		fprintf(stderr, "GPAC Config updated\n");
		return 0;
	}
	if (do_uncache) {
		const char *cache_dir = gf_cfg_get_key(cfg_file, "General", "CacheDirectory");
		do_flatten_cache(cache_dir);
		fprintf(stderr, "GPAC Cache dir %s flattened\n", cache_dir);
		gf_cfg_del(cfg_file);
		return 0;
	}

	if (dump_mode && !url_arg ) {
		FILE *test;
		url_arg = (char *)gf_cfg_get_key(cfg_file, "General", "StartupFile");
		test = url_arg ? gf_fopen(url_arg, "rt") : NULL;
		if (!test) url_arg = NULL;
		else gf_fclose(test);
		
		if (!url_arg) {
			fprintf(stderr, "Missing argument for dump\n");
			PrintUsage();
			if (logfile) gf_fclose(logfile);
			return 1;
		}
	}

	if (!gui_mode && !url_arg && (gf_cfg_get_key(cfg_file, "General", "StartupFile") != NULL)) {
		gui_mode=1;
	}

#ifdef WIN32
	if (gui_mode==1) {
		const char *opt;
		TCHAR buffer[1024];
		DWORD res = GetCurrentDirectory(1024, buffer);
		buffer[res] = 0;
		opt = gf_cfg_get_key(cfg_file, "General", "ModulesDirectory");
		if (strstr(opt, buffer)) {
			gui_mode=1;
		} else {
			gui_mode=2;
		}
	}
#endif

	if (gui_mode==1) {
		hide_shell(1);
	}
	if (gui_mode) {
		no_prog=1;
		gf_set_progress_callback(NULL, progress_quiet);
	}

	if (!url_arg && simulation_time_in_ms)
		simulation_time_in_ms += gf_sys_clock();

#if defined(__DARWIN__) || defined(__APPLE__)
	carbon_init();
#endif


	if (dump_mode) rti_file = NULL;

	if (!logs_set) {
		gf_log_set_tool_level(GF_LOG_ALL, GF_LOG_WARNING);
	}
	if (rti_file || logfile || log_utc_time || log_time_start)
		gf_log_set_callback(NULL, on_gpac_log);

	if (rti_file) init_rti_logs(rti_file, url_arg, use_rtix);

	{
		GF_SystemRTInfo rti;
		if (gf_sys_get_rti(0, &rti, 0))
			fprintf(stderr, "System info: %d MB RAM - %d cores\n", (u32) (rti.physical_memory/1024/1024), rti.nb_cores);
	}


	/*setup dumping options*/
	if (dump_mode) {
		user.init_flags |= GF_TERM_NO_DECODER_THREAD | GF_TERM_NO_COMPOSITOR_THREAD | GF_TERM_NO_REGULATION;
		if (!visible)
			user.init_flags |= GF_TERM_INIT_HIDE;

		gf_cfg_set_key(cfg_file, "Audio", "DriverName", "Raw Audio Output");
		no_cfg_save=GF_TRUE;
	} else {
		init_w = forced_width;
		init_h = forced_height;
	}

	user.modules = gf_modules_new(NULL, cfg_file);
	if (user.modules) i = gf_modules_get_count(user.modules);
	if (!i || !user.modules) {
		fprintf(stderr, "Error: no modules found - exiting\n");
		if (user.modules) gf_modules_del(user.modules);
		gf_cfg_del(cfg_file);
		gf_sys_close();
		if (logfile) gf_fclose(logfile);
		return 1;
	}
	fprintf(stderr, "Modules Found : %d \n", i);

	str = gf_cfg_get_key(cfg_file, "General", "GPACVersion");
	if (!str || strcmp(str, GPAC_FULL_VERSION)) {
		gf_cfg_del_section(cfg_file, "PluginsCache");
		gf_cfg_set_key(cfg_file, "General", "GPACVersion", GPAC_FULL_VERSION);
	}

	user.config = cfg_file;
	user.EventProc = GPAC_EventProc;
	/*dummy in this case (global vars) but MUST be non-NULL*/
	user.opaque = user.modules;
	if (threading_flags) user.init_flags |= threading_flags;
	if (no_audio) user.init_flags |= GF_TERM_NO_AUDIO;
	if (no_regulation) user.init_flags |= GF_TERM_NO_REGULATION;

	if (threading_flags & (GF_TERM_NO_DECODER_THREAD|GF_TERM_NO_COMPOSITOR_THREAD) ) term_step = GF_TRUE;

	if (dump_mode) user.init_flags |= GF_TERM_USE_AUDIO_HW_CLOCK;

	if (bench_mode) {
		gf_cfg_discard_changes(user.config);
		auto_exit = GF_TRUE;
		gf_cfg_set_key(user.config, "Audio", "DriverName", "Raw Audio Output");
		if (bench_mode!=2) {
			gf_cfg_set_key(user.config, "Video", "DriverName", "Raw Video Output");
			gf_cfg_set_key(user.config, "RAWVideo", "RawOutput", "null");
			gf_cfg_set_key(user.config, "Compositor", "OpenGLMode", "disable");
		} else {
			gf_cfg_set_key(user.config, "Video", "DisableVSync", "yes");
		}
	}

	{
		char dim[50];
		sprintf(dim, "%d", forced_width);
		gf_cfg_set_key(user.config, "Compositor", "DefaultWidth", forced_width ? dim : NULL);
		sprintf(dim, "%d", forced_height);
		gf_cfg_set_key(user.config, "Compositor", "DefaultHeight", forced_height ? dim : NULL);
	}

	fprintf(stderr, "Loading GPAC Terminal\n");
	i = gf_sys_clock();
	term = gf_term_new(&user);
	if (!term) {
		fprintf(stderr, "\nInit error - check you have at least one video out and one rasterizer...\nFound modules:\n");
		list_modules(user.modules);
		gf_modules_del(user.modules);
		gf_cfg_discard_changes(cfg_file);
		gf_cfg_del(cfg_file);
		gf_sys_close();
		if (logfile) gf_fclose(logfile);
		return 1;
	}
	fprintf(stderr, "Terminal Loaded in %d ms\n", gf_sys_clock()-i);

	if (bench_mode) {
		display_rti = 2;
		gf_term_set_option(term, GF_OPT_VIDEO_BENCH, (bench_mode==3) ? 2 : 1);
		if (bench_mode==1) bench_mode=2;
	}

	if (dump_mode) {
		if (fill_ar) gf_term_set_option(term, GF_OPT_ASPECT_RATIO, GF_ASPECT_RATIO_FILL_SCREEN);
	} else {
		/*check video output*/
		str = gf_cfg_get_key(cfg_file, "Video", "DriverName");
		if (!bench_mode && !strcmp(str, "Raw Video Output")) fprintf(stderr, "WARNING: using raw output video (memory only) - no display used\n");
		/*check audio output*/
		str = gf_cfg_get_key(cfg_file, "Audio", "DriverName");
		if (!str || !strcmp(str, "No Audio Output Available")) fprintf(stderr, "WARNING: no audio output available - make sure no other program is locking the sound card\n");

		str = gf_cfg_get_key(cfg_file, "General", "NoMIMETypeFetch");
		no_mime_check = (str && !stricmp(str, "yes")) ? 1 : 0;
	}

	str = gf_cfg_get_key(cfg_file, "HTTPProxy", "Enabled");
	if (str && !strcmp(str, "yes")) {
		str = gf_cfg_get_key(cfg_file, "HTTPProxy", "Name");
		if (str) fprintf(stderr, "HTTP Proxy %s enabled\n", str);
	}

	if (rti_file) {
		str = gf_cfg_get_key(cfg_file, "General", "RTIRefreshPeriod");
		if (str) {
			rti_update_time_ms = atoi(str);
		} else {
			gf_cfg_set_key(cfg_file, "General", "RTIRefreshPeriod", "200");
		}
		UpdateRTInfo("At GPAC load time\n");
	}

	Run = 1;

	if (dump_mode) {
		if (!nb_times) {
			times[0] = 0;
			nb_times++;
		}
		ret_val = dump_file(url_arg, out_arg, dump_mode, fps, forced_width, forced_height, scale, times, nb_times);
		Run = 0;
	}
	else if (views) {
	}
	/*connect if requested*/
	else if (!gui_mode && url_arg) {
		char *ext;

		if (strlen(url_arg) >= sizeof(the_url)) {
			fprintf(stderr, "Input url %s is too long, truncating to %d chars.\n", url_arg, (int)(sizeof(the_url) - 1));
			strncpy(the_url, url_arg, sizeof(the_url)-1);
			the_url[sizeof(the_url) - 1] = 0;
		}
		else {
			strcpy(the_url, url_arg);
		}
		ext = strrchr(the_url, '.');
		if (ext && (!stricmp(ext, ".m3u") || !stricmp(ext, ".pls"))) {
			GF_Err e = GF_OK;
			fprintf(stderr, "Opening Playlist %s\n", the_url);

			strcpy(pl_path, the_url);
			/*this is not clean, we need to have a plugin handle playlist for ourselves*/
			if (!strncmp("http:", the_url, 5)) {
				GF_DownloadSession *sess = gf_dm_sess_new(term->downloader, the_url, GF_NETIO_SESSION_NOT_THREADED, NULL, NULL, &e);
				if (sess) {
 					e = gf_dm_sess_process(sess);
 					if (!e) {
 						strncpy(the_url, gf_dm_sess_get_cache_name(sess), sizeof(the_url) - 1);
						the_url[sizeof(the_url) - 1] = 0;
 					}
 					gf_dm_sess_del(sess);
 				}
			}

			playlist = e ? NULL : gf_fopen(the_url, "rt");
			readonly_playlist = 1;
			if (playlist) {
				request_next_playlist_item = GF_TRUE;
			} else {
				if (e)
					fprintf(stderr, "Failed to open playlist %s: %s\n", the_url, gf_error_to_string(e) );
				fprintf(stderr, "Hit 'h' for help\n\n");
			}
		} else {
			fprintf(stderr, "Opening URL %s\n", the_url);
			if (pause_at_first) fprintf(stderr, "[Status: Paused]\n");
			gf_term_connect_from_time(term, the_url, (u64) (play_from*1000), pause_at_first);
		}
	} else {
		fprintf(stderr, "Hit 'h' for help\n\n");
		str = gf_cfg_get_key(cfg_file, "General", "StartupFile");
		if (str) {
			strncpy(the_url, "MP4Client "GPAC_FULL_VERSION , sizeof(the_url)-1);
			the_url[sizeof(the_url) - 1] = 0;
			gf_term_connect(term, str);
			startup_file = 1;
			is_connected = 1;
		}
	}
	if (gui_mode==2) gui_mode=0;

	if (start_fs) gf_term_set_option(term, GF_OPT_FULLSCREEN, 1);

	if (views) {
		char szTemp[4046];
		sprintf(szTemp, "views://%s", views);
		gf_term_connect(term, szTemp);
	}
	if (mosaic) {
		char szTemp[4046];
		sprintf(szTemp, "mosaic://%s", mosaic);
		gf_term_connect(term, szTemp);
	}
	if (bench_mode) {
		rti_update_time_ms = 500;
		bench_mode_start = gf_sys_clock();
	}


	while (Run) {

		/*we don't want getchar to block*/
		if ((gui_mode==1) || !gf_prompt_has_input()) {
			if (reload) {
				reload = 0;
				gf_term_disconnect(term);
				gf_term_connect(term, startup_file ? gf_cfg_get_key(cfg_file, "General", "StartupFile") : the_url);
			}
			if (restart && gf_term_get_option(term, GF_OPT_IS_OVER)) {
				restart = 0;
				gf_term_play_from_time(term, 0, 0);
			}
			if (request_next_playlist_item) {
				c = '\n';
				request_next_playlist_item = 0;
				goto force_input;
			}

			if (has_command && is_connected) {
				has_command = GF_FALSE;
				for (i=0; i<(u32)argc; i++) {
					if (!strcmp(argv[i], "-com")) {
						gf_term_scene_update(term, NULL, argv[i+1]);
						i++;
					}
				}
			}
			if (initial_service_id && is_connected) {
				GF_ObjectManager *root_od = gf_term_get_root_object(term);
				if (root_od) {
					gf_term_select_service(term, root_od, initial_service_id);
					initial_service_id = 0;
				}
			}

			if (!use_rtix || display_rti) UpdateRTInfo(NULL);
			if (term_step) {
				gf_term_process_step(term);
			} else {
				gf_sleep(rti_update_time_ms);
			}
			if (auto_exit && eos_seen && gf_term_get_option(term, GF_OPT_IS_OVER)) {
				Run = GF_FALSE;
			}

			/*sim time*/
			if (simulation_time_in_ms
			        && ( (gf_term_get_elapsed_time_in_ms(term)>simulation_time_in_ms) || (!url_arg && gf_sys_clock()>simulation_time_in_ms))
			   ) {
				Run = GF_FALSE;
			}
			continue;
		}
		c = gf_prompt_get_char();

force_input:
		switch (c) {
		case 'q':
		{
			GF_Event evt;
			memset(&evt, 0, sizeof(GF_Event));
			evt.type = GF_EVENT_QUIT;
			gf_term_send_event(term, &evt);
		}
		break;
		case 'X':
			exit(0);
			break;
		case 'Q':
			break;
		case 'o':
			startup_file = 0;
			gf_term_disconnect(term);
			fprintf(stderr, "Enter the absolute URL\n");
			if (1 > scanf("%s", the_url)) {
				fprintf(stderr, "Cannot read absolute URL, aborting\n");
				break;
			}
			if (rti_file) init_rti_logs(rti_file, the_url, use_rtix);
			gf_term_connect(term, the_url);
			break;
		case 'O':
			gf_term_disconnect(term);
			fprintf(stderr, "Enter the absolute URL to the playlist\n");
			if (1 > scanf("%s", the_url)) {
				fprintf(stderr, "Cannot read the absolute URL, aborting.\n");
				break;
			}
			playlist = gf_fopen(the_url, "rt");
			if (playlist) {
				if (1 >	fscanf(playlist, "%s", the_url)) {
					fprintf(stderr, "Cannot read any URL from playlist, aborting.\n");
					gf_fclose( playlist);
					break;
				}
				fprintf(stderr, "Opening URL %s\n", the_url);
				gf_term_connect(term, the_url);
			}
			break;
		case '\n':
		case 'N':
			if (playlist) {
				int res;
				gf_term_disconnect(term);

				res = fscanf(playlist, "%s", the_url);
				if ((res == EOF) && loop_at_end) {
					fseek(playlist, 0, SEEK_SET);
					res = fscanf(playlist, "%s", the_url);
				}
				if (res == EOF) {
					fprintf(stderr, "No more items - exiting\n");
					Run = 0;
				} else if (the_url[0] == '#') {
					request_next_playlist_item = GF_TRUE;
				} else {
					fprintf(stderr, "Opening URL %s\n", the_url);
					gf_term_connect_with_path(term, the_url, pl_path);
				}
			}
			break;
		case 'P':
			if (playlist) {
				u32 count;
				gf_term_disconnect(term);
				if (1 > scanf("%u", &count)) {
					fprintf(stderr, "Cannot read number, aborting.\n");
					break;
				}
				while (count) {
					if (fscanf(playlist, "%s", the_url)) {
						fprintf(stderr, "Failed to read line, aborting\n");
						break;
					}
					count--;
				}
				fprintf(stderr, "Opening URL %s\n", the_url);
				gf_term_connect(term, the_url);
			}
			break;
		case 'r':
			if (is_connected)
				reload = 1;
			break;

		case 'D':
			if (is_connected) gf_term_disconnect(term);
			break;

		case 'p':
			if (is_connected) {
				Bool is_pause = gf_term_get_option(term, GF_OPT_PLAY_STATE);
				fprintf(stderr, "[Status: %s]\n", is_pause ? "Playing" : "Paused");
				gf_term_set_option(term, GF_OPT_PLAY_STATE, is_pause ? GF_STATE_PLAYING : GF_STATE_PAUSED);
			}
			break;
		case 's':
			if (is_connected) {
				gf_term_set_option(term, GF_OPT_PLAY_STATE, GF_STATE_STEP_PAUSE);
				fprintf(stderr, "Step time: ");
				PrintTime(gf_term_get_time_in_ms(term));
				fprintf(stderr, "\n");
			}
			break;

		case 'z':
		case 'T':
			if (!CanSeek || (Duration<=2000)) {
				fprintf(stderr, "scene not seekable\n");
			} else {
				Double res;
				s32 seekTo;
				fprintf(stderr, "Duration: ");
				PrintTime(Duration);
				res = gf_term_get_time_in_ms(term);
				if (c=='z') {
					res *= 100;
					res /= (s64)Duration;
					fprintf(stderr, " (current %.2f %%)\nEnter Seek percentage:\n", res);
					if (scanf("%d", &seekTo) == 1) {
						if (seekTo > 100) seekTo = 100;
						res = (Double)(s64)Duration;
						res /= 100;
						res *= seekTo;
						gf_term_play_from_time(term, (u64) (s64) res, 0);
					}
				} else {
					u32 r, h, m, s;
					fprintf(stderr, " - Current Time: ");
					PrintTime((u64) res);
					fprintf(stderr, "\nEnter seek time (Format: s, m:s or h:m:s):\n");
					h = m = s = 0;
					r =scanf("%d:%d:%d", &h, &m, &s);
					if (r==2) {
						s = m;
						m = h;
						h = 0;
					}
					else if (r==1) {
						s = h;
						m = h = 0;
					}

					if (r && (r<=3)) {
						u64 time = h*3600 + m*60 + s;
						gf_term_play_from_time(term, time*1000, 0);
					}
				}
			}
			break;

		case 't':
		{
			if (is_connected) {
				fprintf(stderr, "Current Time: ");
				PrintTime(gf_term_get_time_in_ms(term));
				fprintf(stderr, " - Duration: ");
				PrintTime(Duration);
				fprintf(stderr, "\n");
			}
		}
		break;
		case 'w':
			if (is_connected) PrintWorldInfo(term);
			break;
		case 'v':
			if (is_connected) PrintODList(term, NULL, 0, 0, "Root");
			break;
		case 'i':
			if (is_connected) {
				u32 ID;
				fprintf(stderr, "Enter OD ID (0 for main OD): ");
				fflush(stderr);
				if (scanf("%ud", &ID) == 1) {
					ViewOD(term, ID, (u32)-1, NULL);
				} else {
					char str_url[GF_MAX_PATH];
					if (scanf("%s", str_url) == 1)
						ViewOD(term, 0, (u32)-1, str_url);
				}
			}
			break;
		case 'j':
			if (is_connected) {
				u32 num;
				do {
					fprintf(stderr, "Enter OD number (0 for main OD): ");
					fflush(stderr);
				} while( 1 > scanf("%ud", &num));
				ViewOD(term, (u32)-1, num, NULL);
			}
			break;
		case 'b':
			if (is_connected) ViewODs(term, 1);
			break;

		case 'm':
			if (is_connected) ViewODs(term, 0);
			break;

		case 'l':
			list_modules(user.modules);
			break;

		case 'n':
			if (is_connected) set_navigation();
			break;
		case 'x':
			if (is_connected) gf_term_set_option(term, GF_OPT_NAVIGATION_TYPE, 0);
			break;

		case 'd':
			if (is_connected) {
				GF_ObjectManager *odm = NULL;
				char radname[GF_MAX_PATH], *sExt;
				GF_Err e;
				u32 i, count, odid;
				Bool xml_dump, std_out;
				radname[0] = 0;
				do {
					fprintf(stderr, "Enter Inline OD ID if any or 0 : ");
					fflush(stderr);
				} while( 1 >  scanf("%ud", &odid));
				if (odid) {
					GF_ObjectManager *root_odm = gf_term_get_root_object(term);
					if (!root_odm) break;
					count = gf_term_get_object_count(term, root_odm);
					for (i=0; i<count; i++) {
						GF_MediaInfo info;
						odm = gf_term_get_object(term, root_odm, i);
						if (gf_term_get_object_info(term, odm, &info) == GF_OK) {
							if (info.od->objectDescriptorID==odid) break;
						}
						odm = NULL;
					}
				}
				do {
					fprintf(stderr, "Enter file radical name (+\'.x\' for XML dumping) - \"std\" for stderr: ");
					fflush(stderr);
				} while( 1 > scanf("%s", radname));
				sExt = strrchr(radname, '.');
				xml_dump = 0;
				if (sExt) {
					if (!stricmp(sExt, ".x")) xml_dump = 1;
					sExt[0] = 0;
				}
				std_out = strnicmp(radname, "std", 3) ? 0 : 1;
				e = gf_term_dump_scene(term, std_out ? NULL : radname, NULL, xml_dump, 0, odm);
				fprintf(stderr, "Dump done (%s)\n", gf_error_to_string(e));
			}
			break;

		case 'c':
			PrintGPACConfig();
			break;
		case '3':
		{
			Bool use_3d = !gf_term_get_option(term, GF_OPT_USE_OPENGL);
			if (gf_term_set_option(term, GF_OPT_USE_OPENGL, use_3d)==GF_OK) {
				fprintf(stderr, "Using %s for 2D drawing\n", use_3d ? "OpenGL" : "2D rasterizer");
			}
		}
		break;
		case 'k':
		{
			Bool opt = gf_term_get_option(term, GF_OPT_STRESS_MODE);
			opt = !opt;
			fprintf(stderr, "Turning stress mode %s\n", opt ? "on" : "off");
			gf_term_set_option(term, GF_OPT_STRESS_MODE, opt);
		}
		break;
		case '4':
			gf_term_set_option(term, GF_OPT_ASPECT_RATIO, GF_ASPECT_RATIO_4_3);
			break;
		case '5':
			gf_term_set_option(term, GF_OPT_ASPECT_RATIO, GF_ASPECT_RATIO_16_9);
			break;
		case '6':
			gf_term_set_option(term, GF_OPT_ASPECT_RATIO, GF_ASPECT_RATIO_FILL_SCREEN);
			break;
		case '7':
			gf_term_set_option(term, GF_OPT_ASPECT_RATIO, GF_ASPECT_RATIO_KEEP);
			break;

		case 'C':
			switch (gf_term_get_option(term, GF_OPT_MEDIA_CACHE)) {
			case GF_MEDIA_CACHE_DISABLED:
				gf_term_set_option(term, GF_OPT_MEDIA_CACHE, GF_MEDIA_CACHE_ENABLED);
				break;
			case GF_MEDIA_CACHE_ENABLED:
				gf_term_set_option(term, GF_OPT_MEDIA_CACHE, GF_MEDIA_CACHE_DISABLED);
				break;
			case GF_MEDIA_CACHE_RUNNING:
				fprintf(stderr, "Streaming Cache is running - please stop it first\n");
				continue;
			}
			switch (gf_term_get_option(term, GF_OPT_MEDIA_CACHE)) {
			case GF_MEDIA_CACHE_ENABLED:
				fprintf(stderr, "Streaming Cache Enabled\n");
				break;
			case GF_MEDIA_CACHE_DISABLED:
				fprintf(stderr, "Streaming Cache Disabled\n");
				break;
			case GF_MEDIA_CACHE_RUNNING:
				fprintf(stderr, "Streaming Cache Running\n");
				break;
			}
			break;
		case 'S':
		case 'A':
			if (gf_term_get_option(term, GF_OPT_MEDIA_CACHE)==GF_MEDIA_CACHE_RUNNING) {
				gf_term_set_option(term, GF_OPT_MEDIA_CACHE, (c=='S') ? GF_MEDIA_CACHE_DISABLED : GF_MEDIA_CACHE_DISCARD);
				fprintf(stderr, "Streaming Cache stopped\n");
			} else {
				fprintf(stderr, "Streaming Cache not running\n");
			}
			break;
		case 'R':
			display_rti = !display_rti;
			ResetCaption();
			break;
		case 'F':
			if (display_rti) display_rti = 0;
			else display_rti = 2;
			ResetCaption();
			break;

		case 'u':
		{
			GF_Err e;
			char szCom[8192];
			fprintf(stderr, "Enter command to send:\n");
			fflush(stdin);
			szCom[0] = 0;
			if (1 > scanf("%[^\t\n]", szCom)) {
				fprintf(stderr, "Cannot read command to send, aborting.\n");
				break;
			}
			e = gf_term_scene_update(term, NULL, szCom);
			if (e) fprintf(stderr, "Processing command failed: %s\n", gf_error_to_string(e));
		}
		break;
		case 'e':
		{
			GF_Err e;
			char jsCode[8192];
			fprintf(stderr, "Enter JavaScript code to evaluate:\n");
			fflush(stdin);
			jsCode[0] = 0;
			if (1 > scanf("%[^\t\n]", jsCode)) {
				fprintf(stderr, "Cannot read code to evaluate, aborting.\n");
				break;
			}
			e = gf_term_scene_update(term, "application/ecmascript", jsCode);
			if (e) fprintf(stderr, "Processing JS code failed: %s\n", gf_error_to_string(e));
		}
		break;

		case 'L':
		{
			char szLog[1024], *cur_logs;
			cur_logs = gf_log_get_tools_levels();
			fprintf(stderr, "Enter new log level (current tools %s):\n", cur_logs);
			gf_free(cur_logs);
			if (scanf("%s", szLog) < 1) {
				fprintf(stderr, "Cannot read new log level, aborting.\n");
				break;
			}
			gf_log_modify_tools_levels(szLog);
		}
		break;

		case 'g':
		{
			GF_SystemRTInfo rti;
			gf_sys_get_rti(rti_update_time_ms, &rti, 0);
			fprintf(stderr, "GPAC allocated memory "LLD"\n", rti.gpac_memory);
		}
		break;
		case 'M':
		{
			u32 size;
			do {
				fprintf(stderr, "Enter new video cache memory in kBytes (current %ud):\n", gf_term_get_option(term, GF_OPT_VIDEO_CACHE_SIZE));
			} while (1 > scanf("%ud", &size));
			gf_term_set_option(term, GF_OPT_VIDEO_CACHE_SIZE, size);
		}
		break;

		case 'H':
		{
			u32 http_bitrate = gf_term_get_option(term, GF_OPT_HTTP_MAX_RATE);
			do {
				fprintf(stderr, "Enter new http bitrate in bps (0 for none) - current limit: %d\n", http_bitrate);
			} while (1 > scanf("%ud", &http_bitrate));

			gf_term_set_option(term, GF_OPT_HTTP_MAX_RATE, http_bitrate);
		}
		break;

		case 'E':
			gf_term_set_option(term, GF_OPT_RELOAD_CONFIG, 1);
			break;

		case 'B':
			switch_bench(!bench_mode);
			break;

		case 'Y':
		{
			char szOpt[8192];
			fprintf(stderr, "Enter option to set (Section:Name=Value):\n");
			fflush(stdin);
			szOpt[0] = 0;
			if (1 > scanf("%[^\t\n]", szOpt)) {
				fprintf(stderr, "Cannot read option\n");
				break;
			}
			set_cfg_option(szOpt);
		}
		break;

		/*extract to PNG*/
		case 'Z':
		{
			char szFileName[100];
			u32 nb_pass, nb_views, offscreen_view = 0;
			GF_VideoSurface fb;
			GF_Err e;
			nb_pass = 1;
			nb_views = gf_term_get_option(term, GF_OPT_NUM_STEREO_VIEWS);
			if (nb_views>1) {
				fprintf(stderr, "Auto-stereo mode detected - type number of view to dump (0 is main output, 1 to %d offscreen view, %d for all offscreen, %d for all offscreen and main)\n", nb_views, nb_views+1, nb_views+2);
				if (scanf("%d", &offscreen_view) != 1) {
					offscreen_view = 0;
				}
				if (offscreen_view==nb_views+1) {
					offscreen_view = 1;
					nb_pass = nb_views;
				}
				else if (offscreen_view==nb_views+2) {
					offscreen_view = 0;
					nb_pass = nb_views+1;
				}
			}
			while (nb_pass) {
				nb_pass--;
				if (offscreen_view) {
					sprintf(szFileName, "view%d_dump.png", offscreen_view);
					e = gf_term_get_offscreen_buffer(term, &fb, offscreen_view-1, 0);
				} else {
					sprintf(szFileName, "gpac_video_dump_"LLU".png", gf_net_get_utc() );
					e = gf_term_get_screen_buffer(term, &fb);
				}
				offscreen_view++;
				if (e) {
					fprintf(stderr, "Error dumping screen buffer %s\n", gf_error_to_string(e) );
					nb_pass = 0;
				} else {
#ifndef GPAC_DISABLE_AV_PARSERS
					u32 dst_size = fb.width*fb.height*4;
					char *dst = (char*)gf_malloc(sizeof(char)*dst_size);

					e = gf_img_png_enc(fb.video_buffer, fb.width, fb.height, fb.pitch_y, fb.pixel_format, dst, &dst_size);
					if (e) {
						fprintf(stderr, "Error encoding PNG %s\n", gf_error_to_string(e) );
						nb_pass = 0;
					} else {
						FILE *png = gf_fopen(szFileName, "wb");
						if (!png) {
							fprintf(stderr, "Error writing file %s\n", szFileName);
							nb_pass = 0;
						} else {
							gf_fwrite(dst, dst_size, 1, png);
							gf_fclose(png);
							fprintf(stderr, "Dump to %s\n", szFileName);
						}
					}
					if (dst) gf_free(dst);
					gf_term_release_screen_buffer(term, &fb);
#endif //GPAC_DISABLE_AV_PARSERS
				}
			}
			fprintf(stderr, "Done: %s\n", szFileName);
		}
		break;

		case 'G':
		{
			GF_ObjectManager *root_od, *odm;
			u32 index;
			char szOpt[8192];
			fprintf(stderr, "Enter 0-based index of object to select or service ID:\n");
			fflush(stdin);
			szOpt[0] = 0;
			if (1 > scanf("%[^\t\n]", szOpt)) {
				fprintf(stderr, "Cannot read OD ID\n");
				break;
			}
			index = atoi(szOpt);
			odm = NULL;
			root_od = gf_term_get_root_object(term);
			if (root_od) {
				if ( gf_term_find_service(term, root_od, index)) {
					gf_term_select_service(term, root_od, index);
				} else {
					fprintf(stderr, "Cannot find service %d - trying with object index\n", index);
					odm = gf_term_get_object(term, root_od, index);
					if (odm) {
						gf_term_select_object(term, odm);
					} else {
						fprintf(stderr, "Cannot find object at index %d\n", index);
					}
				}
			}
		}
		break;

		case 'h':
			PrintHelp();
			break;
		default:
			break;
		}
	}

	if (bench_mode) {
		PrintAVInfo(GF_TRUE);
	}

	/*FIXME: we have an issue in cleaning up after playing in bench mode and run-for 0 (buildbot tests). We for now disable error checks after run-for is done*/
	if (simulation_time_in_ms) {
		gf_log_set_strict_error(0);
	}


	i = gf_sys_clock();
	gf_term_disconnect(term);
	if (rti_file) UpdateRTInfo("Disconnected\n");

	fprintf(stderr, "Deleting terminal... ");
	if (playlist) gf_fclose(playlist);

#if defined(__DARWIN__) || defined(__APPLE__)
	carbon_uninit();
#endif

	gf_term_del(term);
	fprintf(stderr, "done (in %d ms) - ran for %d ms\n", gf_sys_clock() - i, gf_sys_clock());

	fprintf(stderr, "GPAC cleanup ...\n");
	gf_modules_del(user.modules);

	if (no_cfg_save)
		gf_cfg_discard_changes(cfg_file);

	gf_cfg_del(cfg_file);

	gf_sys_close();

	if (rti_logs) gf_fclose(rti_logs);
	if (logfile) gf_fclose(logfile);

	if (gui_mode) {
		hide_shell(2);
	}

#ifdef GPAC_MEMORY_TRACKING
	if (mem_track && (gf_memory_size() || gf_file_handles_count() )) {
	        gf_log_set_tool_level(GF_LOG_MEMORY, GF_LOG_INFO);
		gf_memory_print();
		return 2;
	}
#endif

	return ret_val;
}
