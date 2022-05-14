struct tcp_conn_t *tcp_conn_accept(struct tcp_sock_t *sock)
struct tcp_conn_t *tcp_conn_select(struct tcp_sock_t *sock,
				   struct tcp_sock_t *sock6)
 {
 	struct tcp_conn_t *conn = calloc(1, sizeof *conn);
 	if (conn == NULL) {
 		ERR("Calloc for connection struct failed");
 		goto error;
 	}
	fd_set rfds;
	struct timeval tv;
	int retval = 0;
	int nfds = 0;
	while (retval == 0) {
		FD_ZERO(&rfds);
		if (sock) {
			FD_SET(sock->sd, &rfds);
			nfds = sock->sd;
		}
		if (sock6) {
			FD_SET(sock6->sd, &rfds);
			if (sock6->sd > nfds)
				nfds = sock6->sd;
		}
		if (nfds == 0) {
			ERR("No valid TCP socket supplied.");
			goto error;
		}
		nfds += 1;
		/* Wait up to five seconds. */
		tv.tv_sec = 5;
		tv.tv_usec = 0;
		retval = select(nfds, &rfds, NULL, NULL, &tv);
		if (retval == -1) {
			ERR("Failed to open tcp connection");
			goto error;
		}
	}
 
	if (sock && FD_ISSET(sock->sd, &rfds)) {
		conn->sd = accept(sock->sd, NULL, NULL);
		NOTE ("Using IPv4");
	} else if (sock6 && FD_ISSET(sock6->sd, &rfds)) {
		conn->sd = accept(sock6->sd, NULL, NULL);
		NOTE ("Using IPv6");
	} else {
		ERR("select failed");
		goto error;
	}
 	if (conn->sd < 0) {
 		ERR("accept failed");
 		goto error;
 	}
 	return conn;
 
 error:
	if (conn != NULL)
		free(conn);
	return NULL;
}
