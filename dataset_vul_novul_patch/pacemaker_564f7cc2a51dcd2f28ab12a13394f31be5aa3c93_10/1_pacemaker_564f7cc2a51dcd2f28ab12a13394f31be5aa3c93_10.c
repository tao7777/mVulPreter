cib_tls_signon(cib_t * cib, struct remote_connection_s *connection)
 {
     int sock;
     cib_remote_opaque_t *private = cib->variant_opaque;
    struct sockaddr_in addr;
     int rc = 0;
    char *server = private->server;
    int ret_ga;
    struct addrinfo *res;
    struct addrinfo hints;
 
     xmlNode *answer = NULL;
     xmlNode *login = NULL;
 
    static struct mainloop_fd_callbacks cib_fd_callbacks = 
        {
            .dispatch = cib_remote_dispatch,
            .destroy = cib_remote_connection_destroy,
        };
 
     connection->socket = 0;
     connection->session = NULL;
 
    /* create socket */
    sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == -1) {
        crm_perror(LOG_ERR, "Socket creation failed");
        return -1;
    }
    /* getaddrinfo */
    bzero(&hints, sizeof(struct addrinfo));
    hints.ai_flags = AI_CANONNAME;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_RAW;
    if (hints.ai_family == AF_INET6) {
        hints.ai_protocol = IPPROTO_ICMPV6;
    } else {
        hints.ai_protocol = IPPROTO_ICMP;
    }
    crm_debug("Looking up %s", server);
    ret_ga = getaddrinfo(server, NULL, &hints, &res);
    if (ret_ga) {
        crm_err("getaddrinfo: %s", gai_strerror(ret_ga));
        close(sock);
        return -1;
    }
    if (res->ai_canonname) {
        server = res->ai_canonname;
    }
    crm_debug("Got address %s for %s", server, private->server);
    if (!res->ai_addr) {
        fprintf(stderr, "getaddrinfo failed");
        crm_exit(1);
    }
#if 1
    memcpy(&addr, res->ai_addr, res->ai_addrlen);
#else
    /* connect to server */
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(server);
#endif
    addr.sin_port = htons(private->port);
    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        crm_perror(LOG_ERR, "Connection to %s:%d failed", server, private->port);
        close(sock);
        return -1;
     }
 
     if (connection->encrypted) {
         /* initialize GnuTls lib */
 #ifdef HAVE_GNUTLS_GNUTLS_H
        gnutls_global_init();
        gnutls_anon_allocate_client_credentials(&anon_cred_c);
 
         /* bind the socket to GnuTls lib */
        connection->session = create_tls_session(sock, GNUTLS_CLIENT);
        if (connection->session == NULL) {
            crm_perror(LOG_ERR, "Session creation for %s:%d failed", server, private->port);
            close(sock);
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
 
    answer = crm_recv_remote_msg(connection->session, connection->encrypted);
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
 
     if (rc != 0) {
         cib_tls_close(cib);
     }
 
    connection->socket = sock;
     connection->source = mainloop_add_fd("cib-remote", G_PRIORITY_HIGH, connection->socket, cib, &cib_fd_callbacks);
     return rc;
 }
