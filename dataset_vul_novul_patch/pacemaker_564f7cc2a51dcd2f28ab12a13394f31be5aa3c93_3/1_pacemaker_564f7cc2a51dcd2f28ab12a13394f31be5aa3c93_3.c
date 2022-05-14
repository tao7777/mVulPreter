cib_remote_msg(gpointer data)
 {
     const char *value = NULL;
    xmlNode *command = NULL;
    cib_client_t *client = data;
    crm_trace("%s callback", client->encrypted ? "secure" : "clear-text");
    command = crm_recv_remote_msg(client->session, client->encrypted);
    if (command == NULL) {
        return -1;
    }
 
     value = crm_element_name(command);
     if (safe_str_neq(value, "cib_command")) {
         crm_log_xml_trace(command, "Bad command: ");
        goto bail;
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
  bail:
    free_xml(command);
    command = NULL;
     return 0;
 }
