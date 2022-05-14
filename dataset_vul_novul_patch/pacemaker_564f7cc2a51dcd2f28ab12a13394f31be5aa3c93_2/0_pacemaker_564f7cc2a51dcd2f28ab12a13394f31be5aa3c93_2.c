 cib_remote_listen(gpointer data)
 {
     int csock = 0;
     unsigned laddr;
     struct sockaddr_in addr;
     int ssock = *(int *)data;
    int flag;
 
 #ifdef HAVE_GNUTLS_GNUTLS_H
     gnutls_session *session = NULL;
 #endif
     cib_client_t *new_client = NULL;
 
     static struct mainloop_fd_callbacks remote_client_fd_callbacks = 
         {
             .dispatch = cib_remote_msg,
             .destroy = cib_remote_connection_destroy,
        };

     /* accept the connection */
     laddr = sizeof(addr);
     csock = accept(ssock, (struct sockaddr *)&addr, &laddr);
    crm_debug("New %s connection from %s",
              ssock == remote_tls_fd ? "secure" : "clear-text", inet_ntoa(addr.sin_addr));

    if (csock == -1) {
        crm_err("accept socket failed");
         return TRUE;
     }
 
    if ((flag = fcntl(csock, F_GETFL)) >= 0) {
        if (fcntl(csock, F_SETFL, flag | O_NONBLOCK) < 0) {
            crm_err( "fcntl() write failed");
            close(csock);
            return TRUE;
        }
    } else {
        crm_err( "fcntl() read failed");
        close(csock);
        return TRUE;
    }

     if (ssock == remote_tls_fd) {
 #ifdef HAVE_GNUTLS_GNUTLS_H
         /* create gnutls session for the server socket */
        session = crm_create_anon_tls_session(csock, GNUTLS_SERVER, anon_cred_s);
         if (session == NULL) {
             crm_err("TLS session creation failed");
             close(csock);
            return TRUE;
        }
 #endif
     }
 
     num_clients++;
     new_client = calloc(1, sizeof(cib_client_t));
     new_client->id = crm_generate_uuid();
     new_client->callback_id = NULL;
    /* clients have a few seconds to perform handshake. */
    new_client->remote_auth_timeout = g_timeout_add(REMOTE_AUTH_TIMEOUT, remote_auth_timeout_cb, new_client);

     if (ssock == remote_tls_fd) {
 #ifdef HAVE_GNUTLS_GNUTLS_H
         new_client->encrypted = TRUE;
        new_client->session = session;
#endif
    } else {
         new_client->session = GINT_TO_POINTER(csock);
     }
 
     new_client->remote = mainloop_add_fd(
         "cib-remote-client", G_PRIORITY_DEFAULT, csock, new_client, &remote_client_fd_callbacks);
 
     g_hash_table_insert(client_list, new_client->id, new_client);
 
     return TRUE;
 }
