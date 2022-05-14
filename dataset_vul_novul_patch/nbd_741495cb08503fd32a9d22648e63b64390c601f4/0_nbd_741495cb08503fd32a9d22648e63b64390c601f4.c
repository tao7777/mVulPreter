void serveloop(GArray* servers) {
	struct sockaddr_storage addrin;
	socklen_t addrinlen=sizeof(addrin);
	int i;
	int max;
	fd_set mset;
	fd_set rset;

	/* 
	 * Set up the master fd_set. The set of descriptors we need
	 * to select() for never changes anyway and it buys us a *lot*
	 * of time to only build this once. However, if we ever choose
	 * to not fork() for clients anymore, we may have to revisit
	 * this.
	 */
	max=0;
	FD_ZERO(&mset);
	for(i=0;i<servers->len;i++) {
		int sock;
		if((sock=(g_array_index(servers, SERVER, i)).socket) >= 0) {
			FD_SET(sock, &mset);
			max=sock>max?sock:max;
		}
	}
	for(i=0;i<modernsocks->len;i++) {
		int sock = g_array_index(modernsocks, int, i);
		FD_SET(sock, &mset);
		max=sock>max?sock:max;
	}
	for(;;) {
                /* SIGHUP causes the root server process to reconfigure
                 * itself and add new export servers for each newly
                 * found export configuration group, i.e. spawn new
                 * server processes for each previously non-existent
                 * export. This does not alter old runtime configuration
                 * but just appends new exports. */
                if (is_sighup_caught) {
                        int n;
                        GError *gerror = NULL;

                        msg(LOG_INFO, "reconfiguration request received");
                        is_sighup_caught = 0; /* Reset to allow catching
                                               * it again. */

                        n = append_new_servers(servers, &gerror);
                        if (n == -1)
                                msg(LOG_ERR, "failed to append new servers: %s",
                                    gerror->message);

                        for (i = servers->len - n; i < servers->len; ++i) {
                                const SERVER server = g_array_index(servers,
                                                                    SERVER, i);

                                if (server.socket >= 0) {
                                        FD_SET(server.socket, &mset);
                                        max = server.socket > max ? server.socket : max;
                                }

                                msg(LOG_INFO, "reconfigured new server: %s",
                                    server.servename);
                        }
                }
 
 		memcpy(&rset, &mset, sizeof(fd_set));
 		if(select(max+1, &rset, NULL, NULL, NULL)>0) {
 
 			DEBUG("accept, ");
 			for(i=0; i < modernsocks->len; i++) {
 				int sock = g_array_index(modernsocks, int, i);
 				if(!FD_ISSET(sock, &rset)) {
 					continue;
 				}
 
				handle_modern_connection(servers, sock);
 			}
 			for(i=0; i < servers->len; i++) {
				int net;
 				SERVER *serve;
 
 				serve=&(g_array_index(servers, SERVER, i));
				if(serve->socket < 0) {
					continue;
				}
				if(FD_ISSET(serve->socket, &rset)) {
					if ((net=accept(serve->socket, (struct sockaddr *) &addrin, &addrinlen)) < 0) {
						err_nonfatal("accept: %m");
						continue;
					}
					handle_connection(servers, net, serve, NULL);
				}
			}
		}
	}
}
