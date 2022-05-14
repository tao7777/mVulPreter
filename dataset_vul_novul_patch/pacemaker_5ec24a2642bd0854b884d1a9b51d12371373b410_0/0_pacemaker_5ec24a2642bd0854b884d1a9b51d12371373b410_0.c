lrmd_remote_listen(gpointer data)
{
    int csock = 0;
    int flag = 0;
    unsigned laddr = 0;
    struct sockaddr addr;
    gnutls_session_t *session = NULL;
    crm_client_t *new_client = NULL;

    static struct mainloop_fd_callbacks lrmd_remote_fd_cb = {
        .dispatch = lrmd_remote_client_msg,
        .destroy = lrmd_remote_client_destroy,
    };

    laddr = sizeof(addr);
    memset(&addr, 0, sizeof(addr));
    getsockname(ssock, &addr, &laddr);

    /* accept the connection */

    if (addr.sa_family == AF_INET6) {
        struct sockaddr_in6 sa;
        char addr_str[INET6_ADDRSTRLEN];

        laddr = sizeof(sa);
        memset(&sa, 0, sizeof(sa));
        csock = accept(ssock, &sa, &laddr);
        get_ip_str((struct sockaddr *)&sa, addr_str, INET6_ADDRSTRLEN);
        crm_info("New remote connection from %s", addr_str);

    } else {
        struct sockaddr_in sa;
        char addr_str[INET_ADDRSTRLEN];

        laddr = sizeof(sa);
        memset(&sa, 0, sizeof(sa));
        csock = accept(ssock, &sa, &laddr);
        get_ip_str((struct sockaddr *)&sa, addr_str, INET_ADDRSTRLEN);
        crm_info("New remote connection from %s", addr_str);
    }

    if (csock == -1) {
        crm_err("accept socket failed");
        return TRUE;
    }

    if ((flag = fcntl(csock, F_GETFL)) >= 0) {
        if (fcntl(csock, F_SETFL, flag | O_NONBLOCK) < 0) {
            crm_err("fcntl() write failed");
            close(csock);
            return TRUE;
        }
    } else {
        crm_err("fcntl() read failed");
        close(csock);
        return TRUE;
    }

    session = create_psk_tls_session(csock, GNUTLS_SERVER, psk_cred_s);
    if (session == NULL) {
        crm_err("TLS session creation failed");
        close(csock);
        return TRUE;
    }

    new_client = calloc(1, sizeof(crm_client_t));
    new_client->remote = calloc(1, sizeof(crm_remote_t));
    new_client->kind = CRM_CLIENT_TLS;
    new_client->remote->tls_session = session;
    new_client->id = crm_generate_uuid();
    new_client->remote->auth_timeout =
        g_timeout_add(LRMD_REMOTE_AUTH_TIMEOUT, lrmd_auth_timeout_cb, new_client);
    crm_notice("LRMD client connection established. %p id: %s", new_client, new_client->id);

    new_client->remote->source =
        mainloop_add_fd("lrmd-remote-client", G_PRIORITY_DEFAULT, csock, new_client,
                         &lrmd_remote_fd_cb);
     g_hash_table_insert(client_connections, new_client->id, new_client);
 
     return TRUE;
 }
