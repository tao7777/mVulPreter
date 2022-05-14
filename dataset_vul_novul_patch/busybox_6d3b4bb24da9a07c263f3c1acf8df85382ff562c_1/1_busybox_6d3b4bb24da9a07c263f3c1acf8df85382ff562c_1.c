int udhcpd_main(int argc UNUSED_PARAM, char **argv)
{
	int server_socket = -1, retval;
	uint8_t *state;
	unsigned timeout_end;
	unsigned num_ips;
	unsigned opt;
	struct option_set *option;
	char *str_I = str_I;
	const char *str_a = "2000";
	unsigned arpping_ms;
	IF_FEATURE_UDHCP_PORT(char *str_P;)

	setup_common_bufsiz();

	IF_FEATURE_UDHCP_PORT(SERVER_PORT = 67;)
	IF_FEATURE_UDHCP_PORT(CLIENT_PORT = 68;)

	opt = getopt32(argv, "^"
		"fSI:va:"IF_FEATURE_UDHCP_PORT("P:")
		"\0"
#if defined CONFIG_UDHCP_DEBUG && CONFIG_UDHCP_DEBUG >= 1
		"vv"
#endif
		, &str_I
		, &str_a
		IF_FEATURE_UDHCP_PORT(, &str_P)
		IF_UDHCP_VERBOSE(, &dhcp_verbose)
		);
	if (!(opt & 1)) { /* no -f */
		bb_daemonize_or_rexec(0, argv);
		logmode = LOGMODE_NONE;
	}
	/* update argv after the possible vfork+exec in daemonize */
	argv += optind;
	if (opt & 2) { /* -S */
		openlog(applet_name, LOG_PID, LOG_DAEMON);
		logmode |= LOGMODE_SYSLOG;
	}
	if (opt & 4) { /* -I */
		len_and_sockaddr *lsa = xhost_and_af2sockaddr(str_I, 0, AF_INET);
		server_config.server_nip = lsa->u.sin.sin_addr.s_addr;
		free(lsa);
	}
#if ENABLE_FEATURE_UDHCP_PORT
	if (opt & 32) { /* -P */
		SERVER_PORT = xatou16(str_P);
		CLIENT_PORT = SERVER_PORT + 1;
	}
#endif
	arpping_ms = xatou(str_a);

	/* Would rather not do read_config before daemonization -
	 * otherwise NOMMU machines will parse config twice */
	read_config(argv[0] ? argv[0] : DHCPD_CONF_FILE);
	/* prevent poll timeout overflow */
	if (server_config.auto_time > INT_MAX / 1000)
		server_config.auto_time = INT_MAX / 1000;

	/* Make sure fd 0,1,2 are open */
	bb_sanitize_stdio();

	/* Create pidfile */
	write_pidfile(server_config.pidfile);
	/* if (!..) bb_perror_msg("can't create pidfile %s", pidfile); */

	bb_error_msg("started, v"BB_VER);

	option = udhcp_find_option(server_config.options, DHCP_LEASE_TIME);
	server_config.max_lease_sec = DEFAULT_LEASE_TIME;
	if (option) {
		move_from_unaligned32(server_config.max_lease_sec, option->data + OPT_DATA);
		server_config.max_lease_sec = ntohl(server_config.max_lease_sec);
	}

	/* Sanity check */
	num_ips = server_config.end_ip - server_config.start_ip + 1;
	if (server_config.max_leases > num_ips) {
		bb_error_msg("max_leases=%u is too big, setting to %u",
			(unsigned)server_config.max_leases, num_ips);
		server_config.max_leases = num_ips;
	}

	/* this sets g_leases */
	SET_PTR_TO_GLOBALS(xzalloc(server_config.max_leases * sizeof(g_leases[0])));

	read_leases(server_config.lease_file);

	if (udhcp_read_interface(server_config.interface,
			&server_config.ifindex,
			(server_config.server_nip == 0 ? &server_config.server_nip : NULL),
			server_config.server_mac)
	) {
		retval = 1;
		goto ret;
	}

	/* Setup the signal pipe */
	udhcp_sp_setup();

 continue_with_autotime:
	timeout_end = monotonic_sec() + server_config.auto_time;
	while (1) { /* loop until universe collapses */
		struct pollfd pfds[2];
		struct dhcp_packet packet;
		int bytes;
		int tv;
		uint8_t *server_id_opt;
		uint8_t *requested_ip_opt;
		uint32_t requested_nip = requested_nip; /* for compiler */
		uint32_t static_lease_nip;
		struct dyn_lease *lease, fake_lease;

		if (server_socket < 0) {
			server_socket = udhcp_listen_socket(/*INADDR_ANY,*/ SERVER_PORT,
					server_config.interface);
		}

		udhcp_sp_fd_set(pfds, server_socket);

 new_tv:
		tv = -1;
		if (server_config.auto_time) {
			tv = timeout_end - monotonic_sec();
			if (tv <= 0) {
 write_leases:
				write_leases();
				goto continue_with_autotime;
			}
			tv *= 1000;
		}

		/* Block here waiting for either signal or packet */
		retval = poll(pfds, 2, tv);
		if (retval <= 0) {
			if (retval == 0)
				goto write_leases;
			if (errno == EINTR)
				goto new_tv;
			/* < 0 and not EINTR: should not happen */
			bb_perror_msg_and_die("poll");
		}

		if (pfds[0].revents) switch (udhcp_sp_read()) {
		case SIGUSR1:
			bb_error_msg("received %s", "SIGUSR1");
			write_leases();
			/* why not just reset the timeout, eh */
			goto continue_with_autotime;
		case SIGTERM:
			bb_error_msg("received %s", "SIGTERM");
			write_leases();
			goto ret0;
		}

		/* Is it a packet? */
		if (!pfds[1].revents)
			continue; /* no */

		/* Note: we do not block here, we block on poll() instead.
		 * Blocking here would prevent SIGTERM from working:
		 * socket read inside this call is restarted on caught signals.
		 */
		bytes = udhcp_recv_kernel_packet(&packet, server_socket);
		if (bytes < 0) {
			/* bytes can also be -2 ("bad packet data") */
			if (bytes == -1 && errno != EINTR) {
				log1("read error: "STRERROR_FMT", reopening socket" STRERROR_ERRNO);
				close(server_socket);
				server_socket = -1;
			}
			continue;
		}
		if (packet.hlen != 6) {
			bb_error_msg("MAC length != 6, ignoring packet");
			continue;
		}
		if (packet.op != BOOTREQUEST) {
			bb_error_msg("not a REQUEST, ignoring packet");
			continue;
		}
		state = udhcp_get_option(&packet, DHCP_MESSAGE_TYPE);
		if (state == NULL || state[0] < DHCP_MINTYPE || state[0] > DHCP_MAXTYPE) {
			bb_error_msg("no or bad message type option, ignoring packet");
			continue;
 		}
 
 		/* Get SERVER_ID if present */
		server_id_opt = udhcp_get_option(&packet, DHCP_SERVER_ID);
 		if (server_id_opt) {
 			uint32_t server_id_network_order;
 			move_from_unaligned32(server_id_network_order, server_id_opt);
			if (server_id_network_order != server_config.server_nip) {
				/* client talks to somebody else */
				log1("server ID doesn't match, ignoring");
				continue;
			}
		}

		/* Look for a static/dynamic lease */
		static_lease_nip = get_static_nip_by_mac(server_config.static_leases, &packet.chaddr);
		if (static_lease_nip) {
			bb_error_msg("found static lease: %x", static_lease_nip);
			memcpy(&fake_lease.lease_mac, &packet.chaddr, 6);
			fake_lease.lease_nip = static_lease_nip;
			fake_lease.expires = 0;
			lease = &fake_lease;
		} else {
			lease = find_lease_by_mac(packet.chaddr);
 		}
 
 		/* Get REQUESTED_IP if present */
		requested_ip_opt = udhcp_get_option(&packet, DHCP_REQUESTED_IP);
 		if (requested_ip_opt) {
 			move_from_unaligned32(requested_nip, requested_ip_opt);
 		}

		switch (state[0]) {

		case DHCPDISCOVER:
			log1("received %s", "DISCOVER");

			send_offer(&packet, static_lease_nip, lease, requested_ip_opt, arpping_ms);
			break;

		case DHCPREQUEST:
			log1("received %s", "REQUEST");
/* RFC 2131:

o DHCPREQUEST generated during SELECTING state:

   Client inserts the address of the selected server in 'server
   identifier', 'ciaddr' MUST be zero, 'requested IP address' MUST be
   filled in with the yiaddr value from the chosen DHCPOFFER.

   Note that the client may choose to collect several DHCPOFFER
   messages and select the "best" offer.  The client indicates its
   selection by identifying the offering server in the DHCPREQUEST
   message.  If the client receives no acceptable offers, the client
   may choose to try another DHCPDISCOVER message.  Therefore, the
   servers may not receive a specific DHCPREQUEST from which they can
   decide whether or not the client has accepted the offer.

o DHCPREQUEST generated during INIT-REBOOT state:

   'server identifier' MUST NOT be filled in, 'requested IP address'
   option MUST be filled in with client's notion of its previously
   assigned address. 'ciaddr' MUST be zero. The client is seeking to
   verify a previously allocated, cached configuration. Server SHOULD
   send a DHCPNAK message to the client if the 'requested IP address'
   is incorrect, or is on the wrong network.

   Determining whether a client in the INIT-REBOOT state is on the
   correct network is done by examining the contents of 'giaddr', the
   'requested IP address' option, and a database lookup. If the DHCP
   server detects that the client is on the wrong net (i.e., the
   result of applying the local subnet mask or remote subnet mask (if
   'giaddr' is not zero) to 'requested IP address' option value
   doesn't match reality), then the server SHOULD send a DHCPNAK
   message to the client.

   If the network is correct, then the DHCP server should check if
   the client's notion of its IP address is correct. If not, then the
   server SHOULD send a DHCPNAK message to the client. If the DHCP
   server has no record of this client, then it MUST remain silent,
   and MAY output a warning to the network administrator. This
   behavior is necessary for peaceful coexistence of non-
   communicating DHCP servers on the same wire.

   If 'giaddr' is 0x0 in the DHCPREQUEST message, the client is on
   the same subnet as the server.  The server MUST broadcast the
   DHCPNAK message to the 0xffffffff broadcast address because the
   client may not have a correct network address or subnet mask, and
   the client may not be answering ARP requests.

   If 'giaddr' is set in the DHCPREQUEST message, the client is on a
   different subnet.  The server MUST set the broadcast bit in the
   DHCPNAK, so that the relay agent will broadcast the DHCPNAK to the
   client, because the client may not have a correct network address
   or subnet mask, and the client may not be answering ARP requests.

o DHCPREQUEST generated during RENEWING state:

   'server identifier' MUST NOT be filled in, 'requested IP address'
   option MUST NOT be filled in, 'ciaddr' MUST be filled in with
   client's IP address. In this situation, the client is completely
   configured, and is trying to extend its lease. This message will
   be unicast, so no relay agents will be involved in its
   transmission.  Because 'giaddr' is therefore not filled in, the
   DHCP server will trust the value in 'ciaddr', and use it when
   replying to the client.

   A client MAY choose to renew or extend its lease prior to T1.  The
   server may choose not to extend the lease (as a policy decision by
   the network administrator), but should return a DHCPACK message
   regardless.

o DHCPREQUEST generated during REBINDING state:

   'server identifier' MUST NOT be filled in, 'requested IP address'
   option MUST NOT be filled in, 'ciaddr' MUST be filled in with
   client's IP address. In this situation, the client is completely
   configured, and is trying to extend its lease. This message MUST
   be broadcast to the 0xffffffff IP broadcast address.  The DHCP
   server SHOULD check 'ciaddr' for correctness before replying to
   the DHCPREQUEST.

   The DHCPREQUEST from a REBINDING client is intended to accommodate
   sites that have multiple DHCP servers and a mechanism for
   maintaining consistency among leases managed by multiple servers.
   A DHCP server MAY extend a client's lease only if it has local
   administrative authority to do so.
*/
			if (!requested_ip_opt) {
				requested_nip = packet.ciaddr;
				if (requested_nip == 0) {
					log1("no requested IP and no ciaddr, ignoring");
					break;
				}
			}
			if (lease && requested_nip == lease->lease_nip) {
				/* client requested or configured IP matches the lease.
				 * ACK it, and bump lease expiration time. */
				send_ACK(&packet, lease->lease_nip);
				break;
			}
			/* No lease for this MAC, or lease IP != requested IP */

			if (server_id_opt    /* client is in SELECTING state */
			 || requested_ip_opt /* client is in INIT-REBOOT state */
			) {
				/* "No, we don't have this IP for you" */
				send_NAK(&packet);
			} /* else: client is in RENEWING or REBINDING, do not answer */

			break;

		case DHCPDECLINE:
			/* RFC 2131:
			 * "If the server receives a DHCPDECLINE message,
			 * the client has discovered through some other means
			 * that the suggested network address is already
			 * in use. The server MUST mark the network address
			 * as not available and SHOULD notify the local
			 * sysadmin of a possible configuration problem."
			 *
			 * SERVER_ID must be present,
			 * REQUESTED_IP must be present,
			 * chaddr must be filled in,
			 * ciaddr must be 0 (we do not check this)
			 */
			log1("received %s", "DECLINE");
			if (server_id_opt
			 && requested_ip_opt
			 && lease  /* chaddr matches this lease */
			 && requested_nip == lease->lease_nip
			) {
				memset(lease->lease_mac, 0, sizeof(lease->lease_mac));
				lease->expires = time(NULL) + server_config.decline_time;
			}
			break;

		case DHCPRELEASE:
			/* "Upon receipt of a DHCPRELEASE message, the server
			 * marks the network address as not allocated."
			 *
			 * SERVER_ID must be present,
			 * REQUESTED_IP must not be present (we do not check this),
			 * chaddr must be filled in,
			 * ciaddr must be filled in
			 */
			log1("received %s", "RELEASE");
			if (server_id_opt
			 && lease  /* chaddr matches this lease */
			 && packet.ciaddr == lease->lease_nip
			) {
				lease->expires = time(NULL);
			}
			break;

		case DHCPINFORM:
			log1("received %s", "INFORM");
			send_inform(&packet);
			break;
		}
	}
 ret0:
	retval = 0;
 ret:
	/*if (server_config.pidfile) - server_config.pidfile is never NULL */
		remove_pidfile(server_config.pidfile);
	return retval;
}
