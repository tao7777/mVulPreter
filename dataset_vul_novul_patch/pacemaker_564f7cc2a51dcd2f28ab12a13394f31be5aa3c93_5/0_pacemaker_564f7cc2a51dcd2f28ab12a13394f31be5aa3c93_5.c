cib_remote_dispatch(gpointer user_data)
cib_remote_command_dispatch(gpointer user_data)
{
    int disconnected = 0;
    cib_t *cib = user_data;
    cib_remote_opaque_t *private = cib->variant_opaque;

    crm_recv_remote_msg(private->command.session, &private->command.recv_buf, private->command.encrypted, -1, &disconnected);

    free(private->command.recv_buf);
    private->command.recv_buf = NULL;
    crm_err("received late reply for remote cib connection, discarding");

    if (disconnected) {
        return -1;
    }
    return 0;
}

int
cib_remote_callback_dispatch(gpointer user_data)
 {
     cib_t *cib = user_data;
     cib_remote_opaque_t *private = cib->variant_opaque;
 
     xmlNode *msg = NULL;
    int disconnected = 0;
 
     crm_info("Message on callback channel");
 
    crm_recv_remote_msg(private->callback.session, &private->callback.recv_buf, private->callback.encrypted, -1, &disconnected);
 
    msg = crm_parse_remote_buffer(&private->callback.recv_buf);
    while (msg) {
        const char *type = crm_element_value(msg, F_TYPE);
        crm_trace("Activating %s callbacks...", type);
 
        if (safe_str_eq(type, T_CIB)) {
            cib_native_callback(cib, msg, 0, 0);
 
        } else if (safe_str_eq(type, T_CIB_NOTIFY)) {
            g_list_foreach(cib->notify_list, cib_native_notify, msg);

        } else {
            crm_err("Unknown message type: %s", type);
        }
 
         free_xml(msg);
        msg = crm_parse_remote_buffer(&private->callback.recv_buf);
    }

    if (disconnected) {
        return -1;
     }

    return 0;
 }
