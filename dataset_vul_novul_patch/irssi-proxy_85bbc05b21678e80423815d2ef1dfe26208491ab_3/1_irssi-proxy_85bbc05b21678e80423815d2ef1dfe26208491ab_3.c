static void server_real_connect(SERVER_REC *server, IPADDR *ip,
				const char *unix_socket)
{
	GIOChannel *handle;
	IPADDR *own_ip = NULL;
	const char *errmsg;
	char *errmsg2;
	char ipaddr[MAX_IP_LEN];
        int port;

	g_return_if_fail(ip != NULL || unix_socket != NULL);

	signal_emit("server connecting", 2, server, ip);

	if (server->connrec->no_connect)
		return;

	if (ip != NULL) {
		own_ip = ip == NULL ? NULL :
			(IPADDR_IS_V6(ip) ? server->connrec->own_ip6 :
			 server->connrec->own_ip4);
 		port = server->connrec->proxy != NULL ?
 			server->connrec->proxy_port : server->connrec->port;
 		handle = server->connrec->use_ssl ?
			net_connect_ip_ssl(ip, port, own_ip, server->connrec->ssl_cert, server->connrec->ssl_pkey,
 server->connrec->ssl_cafile, server->connrec->ssl_capath, server->connrec->ssl_verify) :
 			net_connect_ip(ip, port, own_ip);
 	} else {
		handle = net_connect_unix(unix_socket);
	}

	if (handle == NULL) {
		/* failed */
		errmsg = g_strerror(errno);
		errmsg2 = NULL;
		if (errno == EADDRNOTAVAIL) {
			if (own_ip != NULL) {
				/* show the IP which is causing the error */
				net_ip2host(own_ip, ipaddr);
				errmsg2 = g_strconcat(errmsg, ": ", ipaddr, NULL);
			}
			server->no_reconnect = TRUE;
		}
		if (server->connrec->use_ssl && errno == ENOSYS)
			server->no_reconnect = TRUE;

		server->connection_lost = TRUE;
		server_connect_failed(server, errmsg2 ? errmsg2 : errmsg);
		g_free(errmsg2);
	} else {
		server->handle = net_sendbuffer_create(handle, 0);
#ifdef HAVE_OPENSSL
		if (server->connrec->use_ssl)
			server_connect_callback_init_ssl(server, handle);
		else
#endif
		server->connect_tag =
			g_input_add(handle, G_INPUT_WRITE | G_INPUT_READ,
				    (GInputFunction)
				    server_connect_callback_init,
				    server);
	}
}
