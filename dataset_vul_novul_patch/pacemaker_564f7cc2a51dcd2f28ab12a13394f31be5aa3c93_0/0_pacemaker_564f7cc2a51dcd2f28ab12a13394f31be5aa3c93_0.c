cib_notify_client(gpointer key, gpointer value, gpointer user_data)
{
    const char *type = NULL;
    gboolean do_send = FALSE;

    cib_client_t *client = value;
    xmlNode *update_msg = user_data;

     CRM_CHECK(client != NULL, return TRUE);
     CRM_CHECK(update_msg != NULL, return TRUE);
 
    if (client->ipc == NULL && client->session == NULL) {
         crm_warn("Skipping client with NULL channel");
         return FALSE;
     }

    type = crm_element_value(update_msg, F_SUBTYPE);

    CRM_LOG_ASSERT(type != NULL);
    if (client->diffs && safe_str_eq(type, T_CIB_DIFF_NOTIFY)) {
        do_send = TRUE;

    } else if (client->replace && safe_str_eq(type, T_CIB_REPLACE_NOTIFY)) {
        do_send = TRUE;

    } else if (client->confirmations && safe_str_eq(type, T_CIB_UPDATE_CONFIRM)) {
        do_send = TRUE;

    } else if (client->pre_notify && safe_str_eq(type, T_CIB_PRE_NOTIFY)) {
        do_send = TRUE;

    } else if (client->post_notify && safe_str_eq(type, T_CIB_POST_NOTIFY)) {
        do_send = TRUE;
    }

    if (do_send) {
        if (client->ipc) {
            if(crm_ipcs_send(client->ipc, 0, update_msg, TRUE) == FALSE) {
                crm_warn("Notification of client %s/%s failed", client->name, client->id);
            }

#ifdef HAVE_GNUTLS_GNUTLS_H
        } else if (client->session) {
            crm_debug("Sent %s notification to client %s/%s", type, client->name, client->id);
            crm_send_remote_msg(client->session, update_msg, client->encrypted);

#endif
        } else {
            crm_err("Unknown transport for %s", client->name);
        }
    }
    return FALSE;
}
