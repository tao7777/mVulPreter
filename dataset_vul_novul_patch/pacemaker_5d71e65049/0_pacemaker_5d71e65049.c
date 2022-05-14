 crm_client_new(qb_ipcs_connection_t * c, uid_t uid_client, gid_t gid_client)
 {
     static gid_t gid_cluster = 0;
 
     crm_client_t *client = NULL;

    CRM_LOG_ASSERT(c);
    if (c == NULL) {
        return NULL;
     }
 
     if (gid_cluster == 0) {
         if(crm_user_lookup(CRM_DAEMON_USER, NULL, &gid_cluster) < 0) {
             static bool have_error = FALSE;
             if(have_error == FALSE) {
                crm_warn("Could not find group for user %s", CRM_DAEMON_USER);
                have_error = TRUE;
            }
         }
     }
 
    if (uid_client != 0) {
         crm_trace("Giving access to group %u", gid_cluster);
        /* Passing -1 to chown(2) means don't change */
        qb_ipcs_connection_auth_set(c, -1, gid_cluster, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
     }
 
     crm_client_init();

    /* TODO: Do our own auth checking, return NULL if unauthorized */
    client = calloc(1, sizeof(crm_client_t));

    client->ipcs = c;
    client->kind = CRM_CLIENT_IPC;
    client->pid = crm_ipcs_client_pid(c);

    client->id = crm_generate_uuid();

    crm_debug("Connecting %p for uid=%d gid=%d pid=%u id=%s", c, uid_client, gid_client, client->pid, client->id);

#if ENABLE_ACL
    client->user = uid2username(uid_client);
#endif

    g_hash_table_insert(client_connections, c, client);
    return client;
}
