static int ctdb_tcp_listen_automatic(struct ctdb_context *ctdb)
{
	struct ctdb_tcp *ctcp = talloc_get_type(ctdb->private_data,
                                                struct ctdb_tcp);
         ctdb_sock_addr sock;
        int lock_fd, i;
       const char *lock_path = VARDIR "/run/ctdb/.socket_lock";
        struct flock lock;
        int one = 1;
        int sock_size;
	struct tevent_fd *fde;

	/* If there are no nodes, then it won't be possible to find
	 * the first one.  Log a failure and short circuit the whole
	 * process.
	 */
	if (ctdb->num_nodes == 0) {
		DEBUG(DEBUG_CRIT,("No nodes available to attempt bind to - is the nodes file empty?\n"));
		return -1;
	}

	/* in order to ensure that we don't get two nodes with the
	   same adddress, we must make the bind() and listen() calls
	   atomic. The SO_REUSEADDR setsockopt only prevents double
	   binds if the first socket is in LISTEN state  */
	lock_fd = open(lock_path, O_RDWR|O_CREAT, 0666);
	if (lock_fd == -1) {
		DEBUG(DEBUG_CRIT,("Unable to open %s\n", lock_path));
		return -1;
	}

	lock.l_type = F_WRLCK;
	lock.l_whence = SEEK_SET;
	lock.l_start = 0;
	lock.l_len = 1;
	lock.l_pid = 0;

	if (fcntl(lock_fd, F_SETLKW, &lock) != 0) {
		DEBUG(DEBUG_CRIT,("Unable to lock %s\n", lock_path));
		close(lock_fd);
		return -1;
	}

	for (i=0; i < ctdb->num_nodes; i++) {
		if (ctdb->nodes[i]->flags & NODE_FLAGS_DELETED) {
			continue;
		}
		ZERO_STRUCT(sock);
		if (ctdb_tcp_get_address(ctdb,
				ctdb->nodes[i]->address.address, 
				&sock) != 0) {
			continue;
		}
	
		switch (sock.sa.sa_family) {
		case AF_INET:
			sock.ip.sin_port = htons(ctdb->nodes[i]->address.port);
			sock_size = sizeof(sock.ip);
			break;
		case AF_INET6:
			sock.ip6.sin6_port = htons(ctdb->nodes[i]->address.port);
			sock_size = sizeof(sock.ip6);
			break;
		default:
			DEBUG(DEBUG_ERR, (__location__ " unknown family %u\n",
				sock.sa.sa_family));
			continue;
		}
#ifdef HAVE_SOCK_SIN_LEN
		sock.ip.sin_len = sock_size;
#endif

		ctcp->listen_fd = socket(sock.sa.sa_family, SOCK_STREAM, IPPROTO_TCP);
		if (ctcp->listen_fd == -1) {
			ctdb_set_error(ctdb, "socket failed\n");
			continue;
		}

		set_close_on_exec(ctcp->listen_fd);

	        setsockopt(ctcp->listen_fd,SOL_SOCKET,SO_REUSEADDR,(char *)&one,sizeof(one));

		if (bind(ctcp->listen_fd, (struct sockaddr * )&sock, sock_size) == 0) {
			break;
		}

		if (errno == EADDRNOTAVAIL) {
			DEBUG(DEBUG_DEBUG,(__location__ " Failed to bind() to socket. %s(%d)\n",
					strerror(errno), errno));
		} else {
			DEBUG(DEBUG_ERR,(__location__ " Failed to bind() to socket. %s(%d)\n",
					strerror(errno), errno));
		}
	}
	
	if (i == ctdb->num_nodes) {
		DEBUG(DEBUG_CRIT,("Unable to bind to any of the node addresses - giving up\n"));
		goto failed;
	}
	ctdb->address.address = talloc_strdup(ctdb, ctdb->nodes[i]->address.address);
	ctdb->address.port    = ctdb->nodes[i]->address.port;
	ctdb->name = talloc_asprintf(ctdb, "%s:%u", 
				     ctdb->address.address, 
				     ctdb->address.port);
	ctdb->pnn = ctdb->nodes[i]->pnn;
	DEBUG(DEBUG_INFO,("ctdb chose network address %s:%u pnn %u\n", 
		 ctdb->address.address, 
		 ctdb->address.port, 
		 ctdb->pnn));
	
	if (listen(ctcp->listen_fd, 10) == -1) {
		goto failed;
	}

	fde = event_add_fd(ctdb->ev, ctcp, ctcp->listen_fd, EVENT_FD_READ,
			   ctdb_listen_event, ctdb);
	tevent_fd_set_auto_close(fde);

	close(lock_fd);

	return 0;
	
failed:
	close(lock_fd);
	close(ctcp->listen_fd);
	ctcp->listen_fd = -1;
	return -1;
}
