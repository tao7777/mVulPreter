static int __net_init sctp_net_init(struct net *net)
static int __net_init sctp_defaults_init(struct net *net)
 {
 	int status;
 
	/*
	 * 14. Suggested SCTP Protocol Parameter Values
	 */
	/* The following protocol parameters are RECOMMENDED:  */
	/* RTO.Initial              - 3  seconds */
	net->sctp.rto_initial			= SCTP_RTO_INITIAL;
	/* RTO.Min                  - 1  second */
	net->sctp.rto_min	 		= SCTP_RTO_MIN;
	/* RTO.Max                 -  60 seconds */
	net->sctp.rto_max 			= SCTP_RTO_MAX;
	/* RTO.Alpha                - 1/8 */
	net->sctp.rto_alpha			= SCTP_RTO_ALPHA;
	/* RTO.Beta                 - 1/4 */
	net->sctp.rto_beta			= SCTP_RTO_BETA;

	/* Valid.Cookie.Life        - 60  seconds */
	net->sctp.valid_cookie_life		= SCTP_DEFAULT_COOKIE_LIFE;

	/* Whether Cookie Preservative is enabled(1) or not(0) */
	net->sctp.cookie_preserve_enable 	= 1;

	/* Default sctp sockets to use md5 as their hmac alg */
#if defined (CONFIG_SCTP_DEFAULT_COOKIE_HMAC_MD5)
	net->sctp.sctp_hmac_alg			= "md5";
#elif defined (CONFIG_SCTP_DEFAULT_COOKIE_HMAC_SHA1)
	net->sctp.sctp_hmac_alg			= "sha1";
#else
	net->sctp.sctp_hmac_alg			= NULL;
#endif

	/* Max.Burst		    - 4 */
	net->sctp.max_burst			= SCTP_DEFAULT_MAX_BURST;

	/* Association.Max.Retrans  - 10 attempts
	 * Path.Max.Retrans         - 5  attempts (per destination address)
	 * Max.Init.Retransmits     - 8  attempts
	 */
	net->sctp.max_retrans_association	= 10;
	net->sctp.max_retrans_path		= 5;
	net->sctp.max_retrans_init		= 8;

	/* Sendbuffer growth	    - do per-socket accounting */
	net->sctp.sndbuf_policy			= 0;

	/* Rcvbuffer growth	    - do per-socket accounting */
	net->sctp.rcvbuf_policy			= 0;

	/* HB.interval              - 30 seconds */
	net->sctp.hb_interval			= SCTP_DEFAULT_TIMEOUT_HEARTBEAT;

	/* delayed SACK timeout */
	net->sctp.sack_timeout			= SCTP_DEFAULT_TIMEOUT_SACK;

	/* Disable ADDIP by default. */
	net->sctp.addip_enable = 0;
	net->sctp.addip_noauth = 0;
	net->sctp.default_auto_asconf = 0;

	/* Enable PR-SCTP by default. */
	net->sctp.prsctp_enable = 1;

	/* Disable AUTH by default. */
	net->sctp.auth_enable = 0;

	/* Set SCOPE policy to enabled */
	net->sctp.scope_policy = SCTP_SCOPE_POLICY_ENABLE;

	/* Set the default rwnd update threshold */
	net->sctp.rwnd_upd_shift = SCTP_DEFAULT_RWND_SHIFT;

	/* Initialize maximum autoclose timeout. */
	net->sctp.max_autoclose		= INT_MAX / HZ;

	status = sctp_sysctl_net_register(net);
	if (status)
		goto err_sysctl_register;

	/* Allocate and initialise sctp mibs.  */
	status = init_sctp_mibs(net);
	if (status)
		goto err_init_mibs;

	/* Initialize proc fs directory.  */
	status = sctp_proc_init(net);
	if (status)
		goto err_init_proc;
 
 	sctp_dbg_objcnt_init(net);
 
 	/* Initialize the local address list. */
 	INIT_LIST_HEAD(&net->sctp.local_addr_list);
 	spin_lock_init(&net->sctp.local_addr_lock);
	sctp_get_local_addr_list(net);

	/* Initialize the address event list */
	INIT_LIST_HEAD(&net->sctp.addr_waitq);
	INIT_LIST_HEAD(&net->sctp.auto_asconf_splist);
	spin_lock_init(&net->sctp.addr_wq_lock);
	net->sctp.addr_wq_timer.expires = 0;
	setup_timer(&net->sctp.addr_wq_timer, sctp_addr_wq_timeout_handler,
		    (unsigned long)net);
 
 	return 0;
 
 err_init_proc:
 	cleanup_sctp_mibs(net);
 err_init_mibs:
	sctp_sysctl_net_unregister(net);
err_sysctl_register:
 	return status;
 }
