cib_tls_signon(cib_t * cib, struct remote_connection_s *connection)
cib_tls_signon(cib_t * cib, struct remote_connection_s *connection, gboolean event_channel)
 {
     int sock;
     cib_remote_opaque_t *private = cib->variant_opaque;
     int rc = 0;
    int disconnected = 0;
 
     xmlNode *answer = NULL;
     xmlNode *login = NULL;
 
    static struct mainloop_fd_callbacks cib_fd_callbacks = { 0, };

    cib_fd_callbacks.dispatch = event_channel ? cib_remote_callback_dispatch : cib_remote_command_dispatch;
    cib_fd_callbacks.destroy = cib_remote_connection_destroy;
 
     connection->socket = 0;
     connection->session = NULL;
 
    sock = crm_remote_tcp_connect(private->server, private->port);
    if (sock <= 0) {
        crm_perror(LOG_ERR, "remote tcp connection to %s:%d failed", private->server, private->port);
     }
 
    connection->socket = sock;
     if (connection->encrypted) {
         /* initialize GnuTls lib */
 #ifdef HAVE_GNUTLS_GNUTLS_H
        if (remote_gnutls_credentials_init == FALSE) {
            gnutls_global_init();
            gnutls_anon_allocate_client_credentials(&anon_cred_c);
            remote_gnutls_credentials_init = TRUE;
        }
 
         /* bind the socket to GnuTls lib */
        connection->session = crm_create_anon_tls_session(sock, GNUTLS_CLIENT, anon_cred_c);

        if (crm_initiate_client_tls_handshake(connection->session, DEFAULT_CLIENT_HANDSHAKE_TIMEOUT) != 0) {
            crm_err("Session creation for %s:%d failed", private->server, private->port);

            gnutls_deinit(*connection->session);
            gnutls_free(connection->session);
            connection->session = NULL;
             cib_tls_close(cib);
             return -1;
         }
#else
        return -EPROTONOSUPPORT;
#endif
    } else {
        connection->session = GUINT_TO_POINTER(sock);
    }

    /* login to server */
    login = create_xml_node(NULL, "cib_command");
    crm_xml_add(login, "op", "authenticate");
    crm_xml_add(login, "user", private->user);
    crm_xml_add(login, "password", private->passwd);
    crm_xml_add(login, "hidden", "password");

     crm_send_remote_msg(connection->session, login, connection->encrypted);
     free_xml(login);
 
    crm_recv_remote_msg(connection->session, &connection->recv_buf, connection->encrypted, -1, &disconnected);

    if (disconnected) {
        rc = -ENOTCONN;
    }

    answer = crm_parse_remote_buffer(&connection->recv_buf);

     crm_log_xml_trace(answer, "Reply");
     if (answer == NULL) {
         rc = -EPROTO;

    } else {
        /* grab the token */
        const char *msg_type = crm_element_value(answer, F_CIB_OPERATION);
        const char *tmp_ticket = crm_element_value(answer, F_CIB_CLIENTID);

        if (safe_str_neq(msg_type, CRM_OP_REGISTER)) {
            crm_err("Invalid registration message: %s", msg_type);
            rc = -EPROTO;

        } else if (tmp_ticket == NULL) {
            rc = -EPROTO;

        } else {
             connection->token = strdup(tmp_ticket);
         }
     }
    free_xml(answer);
    answer = NULL;
 
     if (rc != 0) {
         cib_tls_close(cib);
        return rc;
     }
 
    crm_trace("remote client connection established");
     connection->source = mainloop_add_fd("cib-remote", G_PRIORITY_HIGH, connection->socket, cib, &cib_fd_callbacks);
     return rc;
 }
