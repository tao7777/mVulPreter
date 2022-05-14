 cib_remote_connection_destroy(gpointer user_data)
 {
     cib_client_t *client = user_data;
 
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
     free(client->name);
     free(client->callback_id);
     free(client->id);
     free(client->user);
     free(client);
     crm_trace("Freed the cib client");
 
    if (cib_shutdown_flag) {
        cib_shutdown(0);
    }
     return;
 }
