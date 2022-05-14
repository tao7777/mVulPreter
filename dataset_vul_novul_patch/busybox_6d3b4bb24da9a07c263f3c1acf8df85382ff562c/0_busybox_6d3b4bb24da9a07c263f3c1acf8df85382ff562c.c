int udhcpc_main(int argc UNUSED_PARAM, char **argv)
{
	uint8_t *message;
	const char *str_V, *str_h, *str_F, *str_r;
	IF_FEATURE_UDHCPC_ARPING(const char *str_a = "2000";)
	IF_FEATURE_UDHCP_PORT(char *str_P;)
	void *clientid_mac_ptr;
	llist_t *list_O = NULL;
	llist_t *list_x = NULL;
	int tryagain_timeout = 20;
	int discover_timeout = 3;
	int discover_retries = 3;
	uint32_t server_addr = server_addr; /* for compiler */
	uint32_t requested_ip = 0;
	uint32_t xid = xid; /* for compiler */
	int packet_num;
	int timeout; /* must be signed */
	unsigned already_waited_sec;
	unsigned opt;
	IF_FEATURE_UDHCPC_ARPING(unsigned arpping_ms;)
	int retval;

	setup_common_bufsiz();

	/* Default options */
	IF_FEATURE_UDHCP_PORT(SERVER_PORT = 67;)
	IF_FEATURE_UDHCP_PORT(CLIENT_PORT = 68;)
	client_config.interface = "eth0";
	client_config.script = CONFIG_UDHCPC_DEFAULT_SCRIPT;
	str_V = "udhcp "BB_VER;

	/* Parse command line */
	opt = getopt32long(argv, "^"
		/* O,x: list; -T,-t,-A take numeric param */
		"CV:H:h:F:i:np:qRr:s:T:+t:+SA:+O:*ox:*fB"
		USE_FOR_MMU("b")
		IF_FEATURE_UDHCPC_ARPING("a::")
		IF_FEATURE_UDHCP_PORT("P:")
		"v"
		"\0" IF_UDHCP_VERBOSE("vv") /* -v is a counter */
		, udhcpc_longopts
		, &str_V, &str_h, &str_h, &str_F
		, &client_config.interface, &client_config.pidfile /* i,p */
		, &str_r /* r */
		, &client_config.script /* s */
		, &discover_timeout, &discover_retries, &tryagain_timeout /* T,t,A */
		, &list_O
		, &list_x
		IF_FEATURE_UDHCPC_ARPING(, &str_a)
		IF_FEATURE_UDHCP_PORT(, &str_P)
		IF_UDHCP_VERBOSE(, &dhcp_verbose)
	);
	if (opt & (OPT_h|OPT_H)) {
		bb_error_msg("option -h NAME is deprecated, use -x hostname:NAME");
		client_config.hostname = alloc_dhcp_option(DHCP_HOST_NAME, str_h, 0);
	}
	if (opt & OPT_F) {
		/* FQDN option format: [0x51][len][flags][0][0]<fqdn> */
		client_config.fqdn = alloc_dhcp_option(DHCP_FQDN, str_F, 3);
		/* Flag bits: 0000NEOS
		 * S: 1 = Client requests server to update A RR in DNS as well as PTR
		 * O: 1 = Server indicates to client that DNS has been updated regardless
		 * E: 1 = Name is in DNS format, i.e. <4>host<6>domain<3>com<0>,
		 *    not "host.domain.com". Format 0 is obsolete.
		 * N: 1 = Client requests server to not update DNS (S must be 0 then)
		 * Two [0] bytes which follow are deprecated and must be 0.
		 */
		client_config.fqdn[OPT_DATA + 0] = 0x1;
		/*client_config.fqdn[OPT_DATA + 1] = 0; - xzalloc did it */
		/*client_config.fqdn[OPT_DATA + 2] = 0; */
	}
	if (opt & OPT_r)
		requested_ip = inet_addr(str_r);
#if ENABLE_FEATURE_UDHCP_PORT
	if (opt & OPT_P) {
		CLIENT_PORT = xatou16(str_P);
		SERVER_PORT = CLIENT_PORT - 1;
	}
#endif
	IF_FEATURE_UDHCPC_ARPING(arpping_ms = xatou(str_a);)
	while (list_O) {
		char *optstr = llist_pop(&list_O);
		unsigned n = bb_strtou(optstr, NULL, 0);
		if (errno || n > 254) {
			n = udhcp_option_idx(optstr, dhcp_option_strings);
			n = dhcp_optflags[n].code;
		}
		client_config.opt_mask[n >> 3] |= 1 << (n & 7);
	}
	if (!(opt & OPT_o)) {
		unsigned i, n;
		for (i = 0; (n = dhcp_optflags[i].code) != 0; i++) {
			if (dhcp_optflags[i].flags & OPTION_REQ) {
				client_config.opt_mask[n >> 3] |= 1 << (n & 7);
			}
		}
	}
	while (list_x) {
		char *optstr = xstrdup(llist_pop(&list_x));
		udhcp_str2optset(optstr, &client_config.options,
				dhcp_optflags, dhcp_option_strings,
				/*dhcpv6:*/ 0
		);
		free(optstr);
	}

	if (udhcp_read_interface(client_config.interface,
			&client_config.ifindex,
			NULL,
			client_config.client_mac)
	) {
		return 1;
	}

	clientid_mac_ptr = NULL;
	if (!(opt & OPT_C) && !udhcp_find_option(client_config.options, DHCP_CLIENT_ID)) {
		/* not suppressed and not set, set the default client ID */
		client_config.clientid = alloc_dhcp_option(DHCP_CLIENT_ID, "", 7);
		client_config.clientid[OPT_DATA] = 1; /* type: ethernet */
		clientid_mac_ptr = client_config.clientid + OPT_DATA+1;
		memcpy(clientid_mac_ptr, client_config.client_mac, 6);
	}
	if (str_V[0] != '\0') {
		client_config.vendorclass = alloc_dhcp_option(DHCP_VENDOR, str_V, 0);
	}

#if !BB_MMU
	/* on NOMMU reexec (i.e., background) early */
	if (!(opt & OPT_f)) {
		bb_daemonize_or_rexec(0 /* flags */, argv);
		logmode = LOGMODE_NONE;
	}
#endif
	if (opt & OPT_S) {
		openlog(applet_name, LOG_PID, LOG_DAEMON);
		logmode |= LOGMODE_SYSLOG;
	}

	/* Make sure fd 0,1,2 are open */
	bb_sanitize_stdio();
	/* Create pidfile */
	write_pidfile(client_config.pidfile);
	/* Goes to stdout (unless NOMMU) and possibly syslog */
	bb_error_msg("started, v"BB_VER);
	/* Set up the signal pipe */
	udhcp_sp_setup();
	/* We want random_xid to be random... */
	srand(monotonic_us());

	state = INIT_SELECTING;
	udhcp_run_script(NULL, "deconfig");
	change_listen_mode(LISTEN_RAW);
	packet_num = 0;
	timeout = 0;
	already_waited_sec = 0;

	/* Main event loop. select() waits on signal pipe and possibly
	 * on sockfd.
	 * "continue" statements in code below jump to the top of the loop.
	 */
	for (;;) {
		int tv;
		struct pollfd pfds[2];
		struct dhcp_packet packet;
		/* silence "uninitialized!" warning */
		unsigned timestamp_before_wait = timestamp_before_wait;


		/* Was opening raw or udp socket here
		 * if (listen_mode != LISTEN_NONE && sockfd < 0),
		 * but on fast network renew responses return faster
		 * than we open sockets. Thus this code is moved
		 * to change_listen_mode(). Thus we open listen socket
		 * BEFORE we send renew request (see "case BOUND:"). */

		udhcp_sp_fd_set(pfds, sockfd);

		tv = timeout - already_waited_sec;
		retval = 0;
		/* If we already timed out, fall through with retval = 0, else... */
		if (tv > 0) {
			log1("waiting %u seconds", tv);
			timestamp_before_wait = (unsigned)monotonic_sec();
			retval = poll(pfds, 2, tv < INT_MAX/1000 ? tv * 1000 : INT_MAX);
			if (retval < 0) {
				/* EINTR? A signal was caught, don't panic */
				if (errno == EINTR) {
					already_waited_sec += (unsigned)monotonic_sec() - timestamp_before_wait;
					continue;
				}
				/* Else: an error occurred, panic! */
				bb_perror_msg_and_die("poll");
			}
		}

		/* If timeout dropped to zero, time to become active:
		 * resend discover/renew/whatever
		 */
		if (retval == 0) {
			/* When running on a bridge, the ifindex may have changed
			 * (e.g. if member interfaces were added/removed
			 * or if the status of the bridge changed).
			 * Refresh ifindex and client_mac:
			 */
			if (udhcp_read_interface(client_config.interface,
					&client_config.ifindex,
					NULL,
					client_config.client_mac)
			) {
				goto ret0; /* iface is gone? */
			}
			if (clientid_mac_ptr)
				memcpy(clientid_mac_ptr, client_config.client_mac, 6);

			/* We will restart the wait in any case */
			already_waited_sec = 0;

			switch (state) {
			case INIT_SELECTING:
				if (!discover_retries || packet_num < discover_retries) {
					if (packet_num == 0)
						xid = random_xid();
					/* broadcast */
					send_discover(xid, requested_ip);
					timeout = discover_timeout;
					packet_num++;
					continue;
				}
 leasefail:
				udhcp_run_script(NULL, "leasefail");
#if BB_MMU /* -b is not supported on NOMMU */
				if (opt & OPT_b) { /* background if no lease */
					bb_error_msg("no lease, forking to background");
					client_background();
					/* do not background again! */
					opt = ((opt & ~OPT_b) | OPT_f);
				} else
#endif
				if (opt & OPT_n) { /* abort if no lease */
					bb_error_msg("no lease, failing");
					retval = 1;
					goto ret;
				}
				/* wait before trying again */
				timeout = tryagain_timeout;
				packet_num = 0;
				continue;
			case REQUESTING:
				if (packet_num < 3) {
					/* send broadcast select packet */
					send_select(xid, server_addr, requested_ip);
					timeout = discover_timeout;
					packet_num++;
					continue;
				}
				/* Timed out, go back to init state.
				 * "discover...select...discover..." loops
				 * were seen in the wild. Treat them similarly
				 * to "no response to discover" case */
				change_listen_mode(LISTEN_RAW);
				state = INIT_SELECTING;
				goto leasefail;
			case BOUND:
				/* 1/2 lease passed, enter renewing state */
				state = RENEWING;
				client_config.first_secs = 0; /* make secs field count from 0 */
				change_listen_mode(LISTEN_KERNEL);
				log1("entering renew state");
				/* fall right through */
			case RENEW_REQUESTED: /* manual (SIGUSR1) renew */
			case_RENEW_REQUESTED:
			case RENEWING:
				if (timeout >= 60) {
					/* send an unicast renew request */
			/* Sometimes observed to fail (EADDRNOTAVAIL) to bind
			 * a new UDP socket for sending inside send_renew.
			 * I hazard to guess existing listening socket
			 * is somehow conflicting with it, but why is it
			 * not deterministic then?! Strange.
			 * Anyway, it does recover by eventually failing through
			 * into INIT_SELECTING state.
			 */
					if (send_renew(xid, server_addr, requested_ip) >= 0) {
						timeout >>= 1;
						continue;
					}
					/* else: error sending.
					 * example: ENETUNREACH seen with server
					 * which gave us bogus server ID 1.1.1.1
					 * which wasn't reachable (and probably did not exist).
					 */
				}
				/* Timed out or error, enter rebinding state */
				log1("entering rebinding state");
				state = REBINDING;
				/* fall right through */
			case REBINDING:
				/* Switch to bcast receive */
				change_listen_mode(LISTEN_RAW);
				/* Lease is *really* about to run out,
				 * try to find DHCP server using broadcast */
				if (timeout > 0) {
					/* send a broadcast renew request */
					send_renew(xid, 0 /*INADDR_ANY*/, requested_ip);
					timeout >>= 1;
					continue;
				}
				/* Timed out, enter init state */
				bb_error_msg("lease lost, entering init state");
				udhcp_run_script(NULL, "deconfig");
				state = INIT_SELECTING;
				client_config.first_secs = 0; /* make secs field count from 0 */
				/*timeout = 0; - already is */
				packet_num = 0;
				continue;
			/* case RELEASED: */
			}
			/* yah, I know, *you* say it would never happen */
			timeout = INT_MAX;
			continue; /* back to main loop */
		} /* if poll timed out */

		/* poll() didn't timeout, something happened */

		/* Is it a signal? */
		switch (udhcp_sp_read()) {
		case SIGUSR1:
			client_config.first_secs = 0; /* make secs field count from 0 */
			already_waited_sec = 0;
			perform_renew();
			if (state == RENEW_REQUESTED) {
				/* We might be either on the same network
				 * (in which case renew might work),
				 * or we might be on a completely different one
				 * (in which case renew won't ever succeed).
				 * For the second case, must make sure timeout
				 * is not too big, or else we can send
				 * futile renew requests for hours.
				 */
				if (timeout > 60)
					timeout = 60;
				goto case_RENEW_REQUESTED;
			}
			/* Start things over */
			packet_num = 0;
			/* Kill any timeouts, user wants this to hurry along */
			timeout = 0;
			continue;
		case SIGUSR2:
			perform_release(server_addr, requested_ip);
			timeout = INT_MAX;
			continue;
		case SIGTERM:
			bb_error_msg("received %s", "SIGTERM");
			goto ret0;
		}

		/* Is it a packet? */
		if (!pfds[1].revents)
			continue; /* no */

		{
			int len;

			/* A packet is ready, read it */
			if (listen_mode == LISTEN_KERNEL)
				len = udhcp_recv_kernel_packet(&packet, sockfd);
			else
				len = udhcp_recv_raw_packet(&packet, sockfd);
			if (len == -1) {
				/* Error is severe, reopen socket */
				bb_error_msg("read error: "STRERROR_FMT", reopening socket" STRERROR_ERRNO);
				sleep(discover_timeout); /* 3 seconds by default */
				change_listen_mode(listen_mode); /* just close and reopen */
			}
			/* If this packet will turn out to be unrelated/bogus,
			 * we will go back and wait for next one.
			 * Be sure timeout is properly decreased. */
			already_waited_sec += (unsigned)monotonic_sec() - timestamp_before_wait;
			if (len < 0)
				continue;
		}

		if (packet.xid != xid) {
			log1("xid %x (our is %x), ignoring packet",
				(unsigned)packet.xid, (unsigned)xid);
			continue;
		}

		/* Ignore packets that aren't for us */
		if (packet.hlen != 6
		 || memcmp(packet.chaddr, client_config.client_mac, 6) != 0
		) {
			log1("chaddr does not match, ignoring packet"); // log2?
			continue;
		}

		message = udhcp_get_option(&packet, DHCP_MESSAGE_TYPE);
		if (message == NULL) {
			bb_error_msg("no message type option, ignoring packet");
			continue;
		}

		switch (state) {
		case INIT_SELECTING:
			/* Must be a DHCPOFFER */
			if (*message == DHCPOFFER) {
				uint8_t *temp;

/* What exactly is server's IP? There are several values.
 * Example DHCP offer captured with tchdump:
 *
 * 10.34.25.254:67 > 10.34.25.202:68 // IP header's src
 * BOOTP fields:
 * Your-IP 10.34.25.202
 * Server-IP 10.34.32.125   // "next server" IP
 * Gateway-IP 10.34.25.254  // relay's address (if DHCP relays are in use)
 * DHCP options:
 * DHCP-Message Option 53, length 1: Offer
 * Server-ID Option 54, length 4: 10.34.255.7       // "server ID"
 * Default-Gateway Option 3, length 4: 10.34.25.254 // router
 *
 * We think that real server IP (one to use in renew/release)
 * is one in Server-ID option. But I am not 100% sure.
 * IP header's src and Gateway-IP (same in this example)
 * might work too.
 * "Next server" and router are definitely wrong ones to use, though...
 */
/* We used to ignore pcakets without DHCP_SERVER_ID.
 * I've got user reports from people who run "address-less" servers.
 * They either supply DHCP_SERVER_ID of 0.0.0.0 or don't supply it at all.
  * They say ISC DHCP client supports this case.
  */
 				server_addr = 0;
				temp = udhcp_get_option32(&packet, DHCP_SERVER_ID);
 				if (!temp) {
 					bb_error_msg("no server ID, using 0.0.0.0");
 				} else {
					/* it IS unaligned sometimes, don't "optimize" */
					move_from_unaligned32(server_addr, temp);
				}
				/*xid = packet.xid; - already is */
				requested_ip = packet.yiaddr;

				/* enter requesting state */
				state = REQUESTING;
				timeout = 0;
				packet_num = 0;
				already_waited_sec = 0;
			}
			continue;
		case REQUESTING:
		case RENEWING:
		case RENEW_REQUESTED:
		case REBINDING:
			if (*message == DHCPACK) {
				unsigned start;
				uint32_t lease_seconds;
 				struct in_addr temp_addr;
 				uint8_t *temp;
 
				temp = udhcp_get_option32(&packet, DHCP_LEASE_TIME);
 				if (!temp) {
 					bb_error_msg("no lease time with ACK, using 1 hour lease");
 					lease_seconds = 60 * 60;
				} else {
					/* it IS unaligned sometimes, don't "optimize" */
					move_from_unaligned32(lease_seconds, temp);
					lease_seconds = ntohl(lease_seconds);
					/* paranoia: must not be too small and not prone to overflows */
					/* timeout > 60 - ensures at least one unicast renew attempt */
					if (lease_seconds < 2 * 61)
						lease_seconds = 2 * 61;
				}
#if ENABLE_FEATURE_UDHCPC_ARPING
				if (opt & OPT_a) {
/* RFC 2131 3.1 paragraph 5:
 * "The client receives the DHCPACK message with configuration
 * parameters. The client SHOULD perform a final check on the
 * parameters (e.g., ARP for allocated network address), and notes
 * the duration of the lease specified in the DHCPACK message. At this
 * point, the client is configured. If the client detects that the
 * address is already in use (e.g., through the use of ARP),
 * the client MUST send a DHCPDECLINE message to the server and restarts
 * the configuration process..." */
					if (!arpping(packet.yiaddr,
							NULL,
							(uint32_t) 0,
							client_config.client_mac,
							client_config.interface,
							arpping_ms)
					) {
						bb_error_msg("offered address is in use "
							"(got ARP reply), declining");
						send_decline(/*xid,*/ server_addr, packet.yiaddr);

						if (state != REQUESTING)
							udhcp_run_script(NULL, "deconfig");
						change_listen_mode(LISTEN_RAW);
						state = INIT_SELECTING;
						client_config.first_secs = 0; /* make secs field count from 0 */
						requested_ip = 0;
						timeout = tryagain_timeout;
						packet_num = 0;
						already_waited_sec = 0;
						continue; /* back to main loop */
					}
				}
#endif
				/* enter bound state */
				temp_addr.s_addr = packet.yiaddr;
				bb_error_msg("lease of %s obtained, lease time %u",
					inet_ntoa(temp_addr), (unsigned)lease_seconds);
				requested_ip = packet.yiaddr;

				start = monotonic_sec();
				udhcp_run_script(&packet, state == REQUESTING ? "bound" : "renew");
				already_waited_sec = (unsigned)monotonic_sec() - start;
				timeout = lease_seconds / 2;
				if ((unsigned)timeout < already_waited_sec) {
					/* Something went wrong. Back to discover state */
					timeout = already_waited_sec = 0;
				}

				state = BOUND;
				change_listen_mode(LISTEN_NONE);
				if (opt & OPT_q) { /* quit after lease */
					goto ret0;
				}
				/* future renew failures should not exit (JM) */
				opt &= ~OPT_n;
#if BB_MMU /* NOMMU case backgrounded earlier */
				if (!(opt & OPT_f)) {
					client_background();
					/* do not background again! */
					opt = ((opt & ~OPT_b) | OPT_f);
				}
#endif
				/* make future renew packets use different xid */
				/* xid = random_xid(); ...but why bother? */

				continue; /* back to main loop */
			}
			if (*message == DHCPNAK) {
				/* If network has more than one DHCP server,
				 * "wrong" server can reply first, with a NAK.
				 * Do not interpret it as a NAK from "our" server.
				 */
				if (server_addr != 0) {
 					uint32_t svid;
 					uint8_t *temp;
 
					temp = udhcp_get_option32(&packet, DHCP_SERVER_ID);
 					if (!temp) {
  non_matching_svid:
 						log1("received DHCP NAK with wrong"
							" server ID, ignoring packet");
						continue;
					}
					move_from_unaligned32(svid, temp);
					if (svid != server_addr)
						goto non_matching_svid;
				}
				/* return to init state */
				bb_error_msg("received %s", "DHCP NAK");
				udhcp_run_script(&packet, "nak");
				if (state != REQUESTING)
					udhcp_run_script(NULL, "deconfig");
				change_listen_mode(LISTEN_RAW);
				sleep(3); /* avoid excessive network traffic */
				state = INIT_SELECTING;
				client_config.first_secs = 0; /* make secs field count from 0 */
				requested_ip = 0;
				timeout = 0;
				packet_num = 0;
				already_waited_sec = 0;
			}
			continue;
		/* case BOUND: - ignore all packets */
		/* case RELEASED: - ignore all packets */
		}
		/* back to main loop */
	} /* for (;;) - main loop ends */

 ret0:
	if (opt & OPT_R) /* release on quit */
		perform_release(server_addr, requested_ip);
	retval = 0;
 ret:
	/*if (client_config.pidfile) - remove_pidfile has its own check */
		remove_pidfile(client_config.pidfile);
	return retval;
}
