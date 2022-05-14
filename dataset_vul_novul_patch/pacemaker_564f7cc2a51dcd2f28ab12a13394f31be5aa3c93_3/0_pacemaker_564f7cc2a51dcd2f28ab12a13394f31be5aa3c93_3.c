cib_remote_msg(gpointer data)
static void
cib_handle_remote_msg(cib_client_t *client, xmlNode *command)
 {
     const char *value = NULL;
 
     value = crm_element_name(command);
     if (safe_str_neq(value, "cib_command")) {
         crm_log_xml_trace(command, "Bad command: ");
        return;
     }
 
     if (client->name == NULL) {
        value = crm_element_value(command, F_CLIENTNAME);
        if (value == NULL) {
            client->name = strdup(client->id);
        } else {
            client->name = strdup(value);
        }
    }

    if (client->callback_id == NULL) {
        value = crm_element_value(command, F_CIB_CALLBACK_TOKEN);
        if (value != NULL) {
            client->callback_id = strdup(value);
            crm_trace("Callback channel for %s is %s", client->id, client->callback_id);

        } else {
            client->callback_id = strdup(client->id);
        }
    }

    /* unset dangerous options */
    xml_remove_prop(command, F_ORIG);
    xml_remove_prop(command, F_CIB_HOST);
    xml_remove_prop(command, F_CIB_GLOBAL_UPDATE);

    crm_xml_add(command, F_TYPE, T_CIB);
    crm_xml_add(command, F_CIB_CLIENTID, client->id);
    crm_xml_add(command, F_CIB_CLIENTNAME, client->name);
#if ENABLE_ACL
    crm_xml_add(command, F_CIB_USER, client->user);
#endif

    if (crm_element_value(command, F_CIB_CALLID) == NULL) {
        char *call_uuid = crm_generate_uuid();

        /* fix the command */
        crm_xml_add(command, F_CIB_CALLID, call_uuid);
        free(call_uuid);
    }

    if (crm_element_value(command, F_CIB_CALLOPTS) == NULL) {
        crm_xml_add_int(command, F_CIB_CALLOPTS, 0);
    }
 
     crm_log_xml_trace(command, "Remote command: ");
     cib_common_callback_worker(0, 0, command, client, TRUE);
}

int
cib_remote_msg(gpointer data)
{
    xmlNode *command = NULL;
    cib_client_t *client = data;
    int disconnected = 0;
    int timeout = client->remote_auth ? -1 : 1000;

    crm_trace("%s callback", client->encrypted ? "secure" : "clear-text");

#ifdef HAVE_GNUTLS_GNUTLS_H
    if (client->encrypted && (client->handshake_complete == FALSE)) {
        int rc = 0;

        /* Muliple calls to handshake will be required, this callback
         * will be invoked once the client sends more handshake data. */
        do {
            rc = gnutls_handshake(*client->session);

            if (rc < 0 && rc != GNUTLS_E_AGAIN) {
                crm_err("Remote cib tls handshake failed");
                return -1;
            }
        } while (rc == GNUTLS_E_INTERRUPTED);

        if (rc == 0) {
            crm_debug("Remote cib tls handshake completed");
            client->handshake_complete = TRUE;
            if (client->remote_auth_timeout) {
                g_source_remove(client->remote_auth_timeout);
            }
            /* after handshake, clients must send auth in a few seconds */
            client->remote_auth_timeout = g_timeout_add(REMOTE_AUTH_TIMEOUT, remote_auth_timeout_cb, client);
        }
        return 0;
    }
#endif

    crm_recv_remote_msg(client->session, &client->recv_buf, client->encrypted, timeout, &disconnected);

    /* must pass auth before we will process anything else */
    if (client->remote_auth == FALSE) {
        xmlNode *reg;
#if ENABLE_ACL
        const char *user = NULL;
#endif
        command = crm_parse_remote_buffer(&client->recv_buf);
        if (cib_remote_auth(command) == FALSE) {
            free_xml(command);
            return -1;
        }

        crm_debug("remote connection authenticated successfully");
        client->remote_auth = TRUE;
        g_source_remove(client->remote_auth_timeout);
        client->remote_auth_timeout = 0;
        client->name = crm_element_value_copy(command, "name");

#if ENABLE_ACL
        user = crm_element_value(command, "user");
        if (user) {
           new_client->user = strdup(user);
        }
#endif

        /* send ACK */
        reg = create_xml_node(NULL, "cib_result");
        crm_xml_add(reg, F_CIB_OPERATION, CRM_OP_REGISTER);
        crm_xml_add(reg, F_CIB_CLIENTID, client->id);
        crm_send_remote_msg(client->session, reg, client->encrypted);
        free_xml(reg);
        free_xml(command);
    }

    command = crm_parse_remote_buffer(&client->recv_buf);
    while (command) {
        crm_trace("command received");
        cib_handle_remote_msg(client, command);
        free_xml(command);
        command = crm_parse_remote_buffer(&client->recv_buf);
    }

    if (disconnected) {
        crm_trace("disconnected while receiving remote cib msg.");
        return -1;
    }

     return 0;
 }
