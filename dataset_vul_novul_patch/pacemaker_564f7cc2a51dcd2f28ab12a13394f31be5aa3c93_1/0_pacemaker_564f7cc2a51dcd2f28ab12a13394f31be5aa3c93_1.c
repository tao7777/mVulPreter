 cib_remote_connection_destroy(gpointer user_data)
 {
     cib_client_t *client = user_data;
    int csock = 0;
 
     if (client == NULL) {
         return;
    }

    crm_trace("Cleaning up after client disconnect: %s/%s",
              crm_str(client->name), client->id);

    if (client->id != NULL) {
        if (!g_hash_table_remove(client_list, client->id)) {
            crm_err("Client %s not found in the hashtable", client->name);
        }
    }

     crm_trace("Destroying %s (%p)", client->name, user_data);
     num_clients--;
     crm_trace("Num unfree'd clients: %d", num_clients);
    if (client->remote_auth_timeout) {
        g_source_remove(client->remote_auth_timeout);
    }

    if (client->encrypted) {
#ifdef HAVE_GNUTLS_GNUTLS_H
        if (client->session) {
            void *sock_ptr = gnutls_transport_get_ptr(*client->session);
            csock = GPOINTER_TO_INT(sock_ptr);
            if (client->handshake_complete) {
                gnutls_bye(*client->session, GNUTLS_SHUT_WR);
            }
            gnutls_deinit(*client->session);
            gnutls_free(client->session);
        }
#endif
    } else {
        csock = GPOINTER_TO_INT(client->session);
    }
    client->session = NULL;

    if (csock > 0) {
        close(csock);
    }

     free(client->name);
     free(client->callback_id);
     free(client->id);
     free(client->user);
    free(client->recv_buf);
     free(client);
     crm_trace("Freed the cib client");
 
    if (cib_shutdown_flag) {
        cib_shutdown(0);
    }
     return;
 }
