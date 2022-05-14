init_remote_listener(int port, gboolean encrypted)
{
    int rc;
    int *ssock = NULL;
    struct sockaddr_in saddr;
    int optval;
    static struct mainloop_fd_callbacks remote_listen_fd_callbacks = 
        {
            .dispatch = cib_remote_listen,
            .destroy = remote_connection_destroy,
        };

    if (port <= 0) {
        /* dont start it */
        return 0;
    }

    if (encrypted) {
#ifndef HAVE_GNUTLS_GNUTLS_H
        crm_warn("TLS support is not available");
        return 0;
 #else
         crm_notice("Starting a tls listener on port %d.", port);
         gnutls_global_init();
/* 	gnutls_global_set_log_level (10); */
         gnutls_global_set_log_function(debug_log);
         gnutls_dh_params_init(&dh_params);
         gnutls_dh_params_generate2(dh_params, DH_BITS);
        gnutls_anon_allocate_server_credentials(&anon_cred_s);
        gnutls_anon_set_server_dh_params(anon_cred_s, dh_params);
#endif
    } else {
        crm_warn("Starting a plain_text listener on port %d.", port);
    }
#ifndef HAVE_PAM
    crm_warn("PAM is _not_ enabled!");
#endif

    /* create server socket */
    ssock = malloc(sizeof(int));
    *ssock = socket(AF_INET, SOCK_STREAM, 0);
    if (*ssock == -1) {
        crm_perror(LOG_ERR, "Can not create server socket." ERROR_SUFFIX);
        free(ssock);
        return -1;
    }

    /* reuse address */
    optval = 1;
    rc = setsockopt(*ssock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    if(rc < 0) {
        crm_perror(LOG_INFO, "Couldn't allow the reuse of local addresses by our remote listener");
    }

    /* bind server socket */
    memset(&saddr, '\0', sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;
    saddr.sin_port = htons(port);
    if (bind(*ssock, (struct sockaddr *)&saddr, sizeof(saddr)) == -1) {
        crm_perror(LOG_ERR, "Can not bind server socket." ERROR_SUFFIX);
        close(*ssock);
        free(ssock);
        return -2;
    }
    if (listen(*ssock, 10) == -1) {
        crm_perror(LOG_ERR, "Can not start listen." ERROR_SUFFIX);
        close(*ssock);
        free(ssock);
        return -3;
    }

    mainloop_add_fd("cib-remote", G_PRIORITY_DEFAULT, *ssock, ssock, &remote_listen_fd_callbacks);

    return *ssock;
}
