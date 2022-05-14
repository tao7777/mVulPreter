int sandbox(void* sandbox_arg) {
	(void)sandbox_arg;

	pid_t child_pid = getpid();
	if (arg_debug)
		printf("Initializing child process\n");

 	close(parent_to_child_fds[1]);
 	close(child_to_parent_fds[0]);

 	wait_for_other(parent_to_child_fds[0]);

	if (arg_debug && child_pid == 1)
		printf("PID namespace installed\n");


	if (cfg.hostname) {
		if (sethostname(cfg.hostname, strlen(cfg.hostname)) < 0)
			errExit("sethostname");
	}

	if (mount(NULL, "/", NULL, MS_SLAVE | MS_REC, NULL) < 0) {
		chk_chroot();
	}
	preproc_mount_mnt_dir();
	if (mount(LIBDIR "/firejail", RUN_FIREJAIL_LIB_DIR, "none", MS_BIND, NULL) < 0)
		errExit("mounting " RUN_FIREJAIL_LIB_DIR);

	if (cfg.name)
		fs_logger2("sandbox name:", cfg.name);
	fs_logger2int("sandbox pid:", (int) sandbox_pid);
	if (cfg.chrootdir)
		fs_logger("sandbox filesystem: chroot");
	else if (arg_overlay)
		fs_logger("sandbox filesystem: overlay");
	else
		fs_logger("sandbox filesystem: local");
	fs_logger("install mount namespace");

	if (arg_netfilter && any_bridge_configured()) { // assuming by default the client filter
		netfilter(arg_netfilter_file);
	}
	if (arg_netfilter6 && any_bridge_configured()) { // assuming by default the client filter
		netfilter6(arg_netfilter6_file);
	}

	int gw_cfg_failed = 0; // default gw configuration flag
	if (arg_nonetwork) {
		net_if_up("lo");
		if (arg_debug)
			printf("Network namespace enabled, only loopback interface available\n");
	}
	else if (arg_netns) {
		netns(arg_netns);
		if (arg_debug)
			printf("Network namespace '%s' activated\n", arg_netns);
	}
	else if (any_bridge_configured() || any_interface_configured()) {
		net_if_up("lo");

		if (mac_not_zero(cfg.bridge0.macsandbox))
			net_config_mac(cfg.bridge0.devsandbox, cfg.bridge0.macsandbox);
		sandbox_if_up(&cfg.bridge0);

		if (mac_not_zero(cfg.bridge1.macsandbox))
			net_config_mac(cfg.bridge1.devsandbox, cfg.bridge1.macsandbox);
		sandbox_if_up(&cfg.bridge1);

		if (mac_not_zero(cfg.bridge2.macsandbox))
			net_config_mac(cfg.bridge2.devsandbox, cfg.bridge2.macsandbox);
		sandbox_if_up(&cfg.bridge2);

		if (mac_not_zero(cfg.bridge3.macsandbox))
			net_config_mac(cfg.bridge3.devsandbox, cfg.bridge3.macsandbox);
		sandbox_if_up(&cfg.bridge3);


		if (cfg.interface0.configured && cfg.interface0.ip) {
			if (arg_debug)
				printf("Configuring %d.%d.%d.%d address on interface %s\n", PRINT_IP(cfg.interface0.ip), cfg.interface0.dev);
			net_config_interface(cfg.interface0.dev, cfg.interface0.ip, cfg.interface0.mask, cfg.interface0.mtu);
		}
		if (cfg.interface1.configured && cfg.interface1.ip) {
			if (arg_debug)
				printf("Configuring %d.%d.%d.%d address on interface %s\n", PRINT_IP(cfg.interface1.ip), cfg.interface1.dev);
			net_config_interface(cfg.interface1.dev, cfg.interface1.ip, cfg.interface1.mask, cfg.interface1.mtu);
		}
		if (cfg.interface2.configured && cfg.interface2.ip) {
			if (arg_debug)
				printf("Configuring %d.%d.%d.%d address on interface %s\n", PRINT_IP(cfg.interface2.ip), cfg.interface2.dev);
			net_config_interface(cfg.interface2.dev, cfg.interface2.ip, cfg.interface2.mask, cfg.interface2.mtu);
		}
		if (cfg.interface3.configured && cfg.interface3.ip) {
			if (arg_debug)
				printf("Configuring %d.%d.%d.%d address on interface %s\n", PRINT_IP(cfg.interface3.ip), cfg.interface3.dev);
			net_config_interface(cfg.interface3.dev, cfg.interface3.ip, cfg.interface3.mask, cfg.interface3.mtu);
		}

		if (cfg.defaultgw) {
			if (net_add_route(0, 0, cfg.defaultgw)) {
				fwarning("cannot configure default route\n");
				gw_cfg_failed = 1;
			}
		}

		if (arg_debug)
			printf("Network namespace enabled\n");
	}

	if (!arg_quiet) {
		if (any_bridge_configured() || any_interface_configured() || cfg.defaultgw || cfg.dns1) {
			fmessage("\n");
			if (any_bridge_configured() || any_interface_configured()) {
				if (arg_scan)
					sbox_run(SBOX_ROOT | SBOX_CAPS_NETWORK | SBOX_SECCOMP, 3, PATH_FNET, "printif", "scan");
				else
					sbox_run(SBOX_ROOT | SBOX_CAPS_NETWORK | SBOX_SECCOMP, 2, PATH_FNET, "printif");

			}
			if (cfg.defaultgw != 0) {
				if (gw_cfg_failed)
					fmessage("Default gateway configuration failed\n");
				else
					fmessage("Default gateway %d.%d.%d.%d\n", PRINT_IP(cfg.defaultgw));
			}
			if (cfg.dns1 != NULL)
				fmessage("DNS server %s\n", cfg.dns1);
			if (cfg.dns2 != NULL)
				fmessage("DNS server %s\n", cfg.dns2);
			if (cfg.dns3 != NULL)
				fmessage("DNS server %s\n", cfg.dns3);
			if (cfg.dns4 != NULL)
				fmessage("DNS server %s\n", cfg.dns4);
			fmessage("\n");
		}
	}

	if (arg_nonetwork || any_bridge_configured() || any_interface_configured()) {
	}
	else {
		EUID_USER();
		env_ibus_load();
		EUID_ROOT();
	}

#ifdef HAVE_SECCOMP
	if (cfg.protocol) {
		if (arg_debug)
			printf("Build protocol filter: %s\n", cfg.protocol);

		int rv = sbox_run(SBOX_USER | SBOX_CAPS_NONE | SBOX_SECCOMP, 5,
			PATH_FSECCOMP, "protocol", "build", cfg.protocol, RUN_SECCOMP_PROTOCOL);
		if (rv)
			exit(rv);
	}
	if (arg_seccomp && (cfg.seccomp_list || cfg.seccomp_list_drop || cfg.seccomp_list_keep))
		arg_seccomp_postexec = 1;
#endif

	bool need_preload = arg_trace || arg_tracelog || arg_seccomp_postexec;
	if (getuid() != 0 && (arg_appimage || cfg.chrootdir || arg_overlay)) {
		enforce_filters();
		need_preload = arg_trace || arg_tracelog;
	}

	if (need_preload)
		fs_trace_preload();

	if (cfg.hosts_file)
		fs_store_hosts_file();

#ifdef HAVE_CHROOT
	if (cfg.chrootdir) {
		fs_chroot(cfg.chrootdir);

		if (need_preload)
			fs_trace_preload();
	}
	else
#endif
#ifdef HAVE_OVERLAYFS
	if (arg_overlay)
		fs_overlayfs();
	else
#endif
		fs_basic_fs();

	if (arg_private) {
		if (cfg.home_private) {	// --private=
			if (cfg.chrootdir)
				fwarning("private=directory feature is disabled in chroot\n");
			else if (arg_overlay)
				fwarning("private=directory feature is disabled in overlay\n");
			else
				fs_private_homedir();
		}
		else if (cfg.home_private_keep) { // --private-home=
			if (cfg.chrootdir)
				fwarning("private-home= feature is disabled in chroot\n");
			else if (arg_overlay)
				fwarning("private-home= feature is disabled in overlay\n");
			else
				fs_private_home_list();
		}
		else // --private
			fs_private();
	}

	if (arg_private_dev)
		fs_private_dev();

	if (arg_private_etc) {
		if (cfg.chrootdir)
			fwarning("private-etc feature is disabled in chroot\n");
		else if (arg_overlay)
			fwarning("private-etc feature is disabled in overlay\n");
		else {
			fs_private_dir_list("/etc", RUN_ETC_DIR, cfg.etc_private_keep);
			if (need_preload)
				fs_trace_preload();
		}
	}

	if (arg_private_opt) {
		if (cfg.chrootdir)
			fwarning("private-opt feature is disabled in chroot\n");
		else if (arg_overlay)
			fwarning("private-opt feature is disabled in overlay\n");
		else {
			fs_private_dir_list("/opt", RUN_OPT_DIR, cfg.opt_private_keep);
		}
	}

	if (arg_private_srv) {
		if (cfg.chrootdir)
			fwarning("private-srv feature is disabled in chroot\n");
		else if (arg_overlay)
			fwarning("private-srv feature is disabled in overlay\n");
		else {
			fs_private_dir_list("/srv", RUN_SRV_DIR, cfg.srv_private_keep);
		}
	}

	if (arg_private_bin && !arg_appimage) {
		if (cfg.chrootdir)
			fwarning("private-bin feature is disabled in chroot\n");
		else if (arg_overlay)
			fwarning("private-bin feature is disabled in overlay\n");
		else {
			if (arg_x11_xorg) {
				EUID_USER();
				char *tmp;
				if (asprintf(&tmp, "%s,xauth", cfg.bin_private_keep) == -1)
					errExit("asprintf");
				cfg.bin_private_keep = tmp;
				EUID_ROOT();
			}
			fs_private_bin_list();
		}
	}

	if (arg_private_lib && !arg_appimage) {
		if (cfg.chrootdir)
			fwarning("private-lib feature is disabled in chroot\n");
		else if (arg_overlay)
			fwarning("private-lib feature is disabled in overlay\n");
		else {
			fs_private_lib();
		}
	}

	if (arg_private_cache) {
		if (cfg.chrootdir)
			fwarning("private-cache feature is disabled in chroot\n");
		else if (arg_overlay)
			fwarning("private-cache feature is disabled in overlay\n");
		else
			fs_private_cache();
	}

	if (arg_private_tmp) {
		EUID_USER();
		fs_private_tmp();
		EUID_ROOT();
	}

	if (arg_nodbus)
		dbus_session_disable();


	if (cfg.hostname)
		fs_hostname(cfg.hostname);

	if (cfg.hosts_file)
		fs_mount_hosts_file();

	if (arg_netns)
		netns_mounts(arg_netns);

	fs_proc_sys_dev_boot();

	if (checkcfg(CFG_DISABLE_MNT))
		fs_mnt(1);
	else if (arg_disable_mnt)
		fs_mnt(0);

	fs_whitelist();

	fs_blacklist(); // mkdir and mkfile are processed all over again

	if (arg_nosound) {
		pulseaudio_disable();

		fs_dev_disable_sound();
	}
	else if (!arg_noautopulse)
		pulseaudio_init();

	if (arg_no3d)
		fs_dev_disable_3d();

	if (arg_notv)
		fs_dev_disable_tv();

	if (arg_nodvd)
		fs_dev_disable_dvd();

	if (arg_nou2f)
	        fs_dev_disable_u2f();

	if (arg_novideo)
		fs_dev_disable_video();

	if (need_preload)
		fs_trace();

	fs_resolvconf();

	fs_logger_print();
	fs_logger_change_owner();

	EUID_USER();
	int cwd = 0;
	if (cfg.cwd) {
		if (chdir(cfg.cwd) == 0)
			cwd = 1;
	}

	if (!cwd) {
		if (chdir("/") < 0)
			errExit("chdir");
		if (cfg.homedir) {
			struct stat s;
			if (stat(cfg.homedir, &s) == 0) {
				/* coverity[toctou] */
				if (chdir(cfg.homedir) < 0)
					errExit("chdir");
			}
		}
	}
	if (arg_debug) {
		char *cpath = get_current_dir_name();
		if (cpath) {
			printf("Current directory: %s\n", cpath);
			free(cpath);
		}
	}

	EUID_ROOT();
	fs_x11();
	if (arg_x11_xorg)
		x11_xorg();

	save_umask();

 	save_nonewprivs();
 
 	save_cpu();
 
	save_cgroup();

#ifdef HAVE_SECCOMP
#ifdef SYS_socket
	if (cfg.protocol) {
		if (arg_debug)
			printf("Install protocol filter: %s\n", cfg.protocol);
		seccomp_load(RUN_SECCOMP_PROTOCOL);	// install filter
		protocol_filter_save();	// save filter in RUN_PROTOCOL_CFG
	}
	else {
		int rv = unlink(RUN_SECCOMP_PROTOCOL);
		(void) rv;
	}
#endif

	if (arg_seccomp == 1) {
		if (cfg.seccomp_list_keep)
			seccomp_filter_keep();
		else
			seccomp_filter_drop();

	}
	else { // clean seccomp files under /run/firejail/mnt
		int rv = unlink(RUN_SECCOMP_CFG);
		rv |= unlink(RUN_SECCOMP_32);
		(void) rv;
	}

	if (arg_memory_deny_write_execute) {
		if (arg_debug)
			printf("Install memory write&execute filter\n");
		seccomp_load(RUN_SECCOMP_MDWX);	// install filter
	}
	else {
 		int rv = unlink(RUN_SECCOMP_MDWX);
 		(void) rv;
 	}
	// make seccomp filters read-only
	fs_rdonly(RUN_SECCOMP_DIR);
 #endif
 
	// set capabilities
	set_caps();


	FILE *rj = create_ready_for_join_file();

	save_nogroups();
	if (arg_noroot) {
		int rv = unshare(CLONE_NEWUSER);
		if (rv == -1) {
			fwarning("cannot create a new user namespace, going forward without it...\n");
			arg_noroot = 0;
		}
	}

 	notify_other(child_to_parent_fds[1]);
 	close(child_to_parent_fds[1]);

 	wait_for_other(parent_to_child_fds[0]);
 	close(parent_to_child_fds[0]);

	if (arg_noroot) {
		if (arg_debug)
			printf("noroot user namespace installed\n");
		set_caps();
	}

	if (arg_nonewprivs) {
		prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0);

		if (prctl(PR_GET_NO_NEW_PRIVS, 0, 0, 0, 0) != 1) {
			fwarning("cannot set NO_NEW_PRIVS, it requires a Linux kernel version 3.5 or newer.\n");
			if (force_nonewprivs) {
				fprintf(stderr, "Error: NO_NEW_PRIVS required for this sandbox, exiting ...\n");
				exit(1);
			}
		}
		else if (arg_debug)
			printf("NO_NEW_PRIVS set\n");
	}

	drop_privs(arg_nogroups);

	prctl(PR_SET_PDEATHSIG, SIGKILL, 0, 0, 0);


	if (cfg.cpus)
		set_cpu_affinity();

	pid_t app_pid = fork();
	if (app_pid == -1)
		errExit("fork");

	if (app_pid == 0) {
#ifdef HAVE_APPARMOR
		if (checkcfg(CFG_APPARMOR) && arg_apparmor) {
			errno = 0;
			if (aa_change_onexec("firejail-default")) {
				fwarning("Cannot confine the application using AppArmor.\n"
					"Maybe firejail-default AppArmor profile is not loaded into the kernel.\n"
					"As root, run \"aa-enforce firejail-default\" to load it.\n");
			}
			else if (arg_debug)
				printf("AppArmor enabled\n");
		}
#endif
		if (arg_nice)
			set_nice(cfg.nice);
		set_rlimits();

		start_application(0, rj);
	}

	fclose(rj);

	int status = monitor_application(app_pid);	// monitor application
	flush_stdin();

	if (WIFEXITED(status)) {
		return WEXITSTATUS(status);
	} else {
		return -1;
	}
}
