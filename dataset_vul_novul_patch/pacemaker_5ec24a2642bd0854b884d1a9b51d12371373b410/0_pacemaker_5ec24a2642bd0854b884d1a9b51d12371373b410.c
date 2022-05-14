lrmd_remote_client_msg(gpointer data)
{
    int id = 0;
    int rc = 0;
    int disconnected = 0;
    xmlNode *request = NULL;
    crm_client_t *client = data;

    if (client->remote->tls_handshake_complete == FALSE) {
        int rc = 0;

        /* Muliple calls to handshake will be required, this callback
         * will be invoked once the client sends more handshake data. */
        do {
            rc = gnutls_handshake(*client->remote->tls_session);

            if (rc < 0 && rc != GNUTLS_E_AGAIN) {
                crm_err("Remote lrmd tls handshake failed");
                return -1;
            }
        } while (rc == GNUTLS_E_INTERRUPTED);

        if (rc == 0) {
            crm_debug("Remote lrmd tls handshake completed");
            client->remote->tls_handshake_complete = TRUE;
            if (client->remote->auth_timeout) {
                 g_source_remove(client->remote->auth_timeout);
             }
             client->remote->auth_timeout = 0;

            /* Alert other clients of the new connection */
            notify_of_new_client(client);
         }
         return 0;
     }

    rc = crm_remote_ready(client->remote, 0);
    if (rc == 0) {
        /* no msg to read */
        return 0;
    } else if (rc < 0) {
        crm_info("Client disconnected during remote client read");
        return -1;
    }

    crm_remote_recv(client->remote, -1, &disconnected);

    request = crm_remote_parse_buffer(client->remote);
    while (request) {
        crm_element_value_int(request, F_LRMD_REMOTE_MSG_ID, &id);
        crm_trace("processing request from remote client with remote msg id %d", id);
        if (!client->name) {
            const char *value = crm_element_value(request, F_LRMD_CLIENTNAME);

            if (value) {
                client->name = strdup(value);
            }
        }

        lrmd_call_id++;
        if (lrmd_call_id < 1) {
            lrmd_call_id = 1;
        }

        crm_xml_add(request, F_LRMD_CLIENTID, client->id);
        crm_xml_add(request, F_LRMD_CLIENTNAME, client->name);
        crm_xml_add_int(request, F_LRMD_CALLID, lrmd_call_id);

        process_lrmd_message(client, id, request);
        free_xml(request);

        /* process all the messages in the current buffer */
        request = crm_remote_parse_buffer(client->remote);
    }

    if (disconnected) {
        crm_info("Client disconnect detected in tls msg dispatcher.");
        return -1;
    }

    return 0;
}
