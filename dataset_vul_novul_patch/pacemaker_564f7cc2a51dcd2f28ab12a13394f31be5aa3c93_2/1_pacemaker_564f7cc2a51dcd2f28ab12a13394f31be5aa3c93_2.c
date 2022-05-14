 cib_remote_listen(gpointer data)
 {
    int lpc = 0;
     int csock = 0;
     unsigned laddr;
    time_t now = 0;
    time_t start = time(NULL);
     struct sockaddr_in addr;
     int ssock = *(int *)data;
 
 #ifdef HAVE_GNUTLS_GNUTLS_H
     gnutls_session *session = NULL;
 #endif
     cib_client_t *new_client = NULL;
 
    xmlNode *login = NULL;
    const char *user = NULL;
    const char *pass = NULL;
    const char *tmp = NULL;
#ifdef HAVE_DECL_NANOSLEEP
    const struct timespec sleepfast = { 0, 10000000 };  /* 10 millisec */
#endif
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
 
     if (ssock == remote_tls_fd) {
 #ifdef HAVE_GNUTLS_GNUTLS_H
         /* create gnutls session for the server socket */
        session = create_tls_session(csock, GNUTLS_SERVER);
         if (session == NULL) {
             crm_err("TLS session creation failed");
             close(csock);
            return TRUE;
        }
 #endif
     }
 
    do {
        crm_trace("Iter: %d", lpc++);
        if (ssock == remote_tls_fd) {
#ifdef HAVE_GNUTLS_GNUTLS_H
            login = crm_recv_remote_msg(session, TRUE);
#endif
        } else {
            login = crm_recv_remote_msg(GINT_TO_POINTER(csock), FALSE);
        }
        if (login != NULL) {
            break;
        }
#ifdef HAVE_DECL_NANOSLEEP
        nanosleep(&sleepfast, NULL);
#else
        sleep(1);
#endif
        now = time(NULL);
        /* Peers have 3s to connect */
    } while (login == NULL && (start - now) < 4);
    crm_log_xml_info(login, "Login: ");
    if (login == NULL) {
        goto bail;
    }
    tmp = crm_element_name(login);
    if (safe_str_neq(tmp, "cib_command")) {
        crm_err("Wrong tag: %s", tmp);
        goto bail;
    }
    tmp = crm_element_value(login, "op");
    if (safe_str_neq(tmp, "authenticate")) {
        crm_err("Wrong operation: %s", tmp);
        goto bail;
    }
    user = crm_element_value(login, "user");
    pass = crm_element_value(login, "password");
    /* Non-root daemons can only validate the password of the
     * user they're running as
     */
    if (check_group_membership(user, CRM_DAEMON_GROUP) == FALSE) {
        crm_err("User is not a member of the required group");
        goto bail;
    } else if (authenticate_user(user, pass) == FALSE) {
        crm_err("PAM auth failed");
        goto bail;
    }
    /* send ACK */
     num_clients++;
     new_client = calloc(1, sizeof(cib_client_t));
    new_client->name = crm_element_value_copy(login, "name");
    CRM_CHECK(new_client->id == NULL, free(new_client->id));
     new_client->id = crm_generate_uuid();
#if ENABLE_ACL
    new_client->user = strdup(user);
#endif
     new_client->callback_id = NULL;
     if (ssock == remote_tls_fd) {
 #ifdef HAVE_GNUTLS_GNUTLS_H
         new_client->encrypted = TRUE;
        new_client->session = session;
#endif
    } else {
         new_client->session = GINT_TO_POINTER(csock);
     }
 
    free_xml(login);
    login = create_xml_node(NULL, "cib_result");
    crm_xml_add(login, F_CIB_OPERATION, CRM_OP_REGISTER);
    crm_xml_add(login, F_CIB_CLIENTID, new_client->id);
    crm_send_remote_msg(new_client->session, login, new_client->encrypted);
    free_xml(login);
     new_client->remote = mainloop_add_fd(
         "cib-remote-client", G_PRIORITY_DEFAULT, csock, new_client, &remote_client_fd_callbacks);
 
     g_hash_table_insert(client_list, new_client->id, new_client);
 
     return TRUE;
  bail:
    if (ssock == remote_tls_fd) {
#ifdef HAVE_GNUTLS_GNUTLS_H
        gnutls_bye(*session, GNUTLS_SHUT_RDWR);
        gnutls_deinit(*session);
        gnutls_free(session);
#endif
    }
    close(csock);
    free_xml(login);
    return TRUE;
 }
