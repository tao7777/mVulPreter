do_local_notify(xmlNode * notify_src, const char *client_id,
                gboolean sync_reply, gboolean from_peer)
{
    /* send callback to originating child */
    cib_client_t *client_obj = NULL;
    int local_rc = pcmk_ok;

    if (client_id != NULL) {
        client_obj = g_hash_table_lookup(client_list, client_id);
    } else {
        crm_trace("No client to sent the response to. F_CIB_CLIENTID not set.");
    }

    if (client_obj == NULL) {
        local_rc = -ECONNRESET;

    } else {
         int rid = 0;
 
         if(sync_reply) {
            CRM_LOG_ASSERT(client_obj->request_id);
 
            rid = client_obj->request_id;
            client_obj->request_id = 0;
 
            crm_trace("Sending response %d to %s %s",
                       rid, client_obj->name, from_peer?"(originator of delegated request)":"");
 
         } else {
             crm_trace("Sending an event to %s %s",
                      client_obj->name, from_peer?"(originator of delegated request)":"");
        }

        if (client_obj->ipc && crm_ipcs_send(client_obj->ipc, rid, notify_src, !sync_reply) < 0) {
            local_rc = -ENOMSG;

#ifdef HAVE_GNUTLS_GNUTLS_H
        } else if (client_obj->session) {
            crm_send_remote_msg(client_obj->session, notify_src, client_obj->encrypted);
#endif
        } else if(client_obj->ipc == NULL) {
            crm_err("Unknown transport for %s", client_obj->name);
        }
    }

    if (local_rc != pcmk_ok && client_obj != NULL) {
        crm_warn("%sSync reply to %s failed: %s",
                 sync_reply ? "" : "A-",
                 client_obj ? client_obj->name : "<unknown>", pcmk_strerror(local_rc));
    }
}
