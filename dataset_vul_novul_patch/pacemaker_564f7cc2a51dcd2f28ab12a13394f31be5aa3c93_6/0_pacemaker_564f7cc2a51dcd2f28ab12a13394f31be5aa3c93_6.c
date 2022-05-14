 cib_remote_perform_op(cib_t * cib, const char *op, const char *host, const char *section,
                       xmlNode * data, xmlNode ** output_data, int call_options, const char *name)
 {
     int rc = pcmk_ok;
    int disconnected = 0;
    int remaining_time = 0;
    time_t start_time;
 
     xmlNode *op_msg = NULL;
     xmlNode *op_reply = NULL;
 
     cib_remote_opaque_t *private = cib->variant_opaque;
 
     if (cib->state == cib_disconnected) {
         return -ENOTCONN;
     }

    if (output_data != NULL) {
        *output_data = NULL;
    }

    if (op == NULL) {
        crm_err("No operation specified");
        return -EINVAL;
    }

    cib->call_id++;
    /* prevent call_id from being negative (or zero) and conflicting
     *    with the cib_errors enum
     * use 2 because we use it as (cib->call_id - 1) below
     */
    if (cib->call_id < 1) {
        cib->call_id = 1;
    }

    op_msg =
        cib_create_op(cib->call_id, private->callback.token, op, host, section, data, call_options,
                      NULL);
    if (op_msg == NULL) {
        return -EPROTO;
     }
 
     crm_trace("Sending %s message to CIB service", op);
    if (!(call_options & cib_sync_call)) {
        crm_send_remote_msg(private->callback.session, op_msg, private->command.encrypted);
    } else {
        crm_send_remote_msg(private->command.session, op_msg, private->command.encrypted);
    }
     free_xml(op_msg);
 
     if ((call_options & cib_discard_reply)) {
        crm_trace("Discarding reply");
        return pcmk_ok;

    } else if (!(call_options & cib_sync_call)) {
        return cib->call_id;
    }
 
     crm_trace("Waiting for a syncronous reply");
 
    start_time = time(NULL);
    remaining_time = cib->call_timeout ? cib->call_timeout : 60;
 
    while (remaining_time > 0 && !disconnected) {
         int reply_id = -1;
         int msg_id = cib->call_id;
 
        crm_recv_remote_msg(private->command.session, &private->command.recv_buf, private->command.encrypted, remaining_time * 1000, &disconnected);
        op_reply = crm_parse_remote_buffer(&private->command.recv_buf);

        if (!op_reply) {
             break;
         }
 
         crm_element_value_int(op_reply, F_CIB_CALLID, &reply_id);
 
         if (reply_id == msg_id) {
             break;

        } else if (reply_id < msg_id) {
            crm_debug("Received old reply: %d (wanted %d)", reply_id, msg_id);
            crm_log_xml_trace(op_reply, "Old reply");

        } else if ((reply_id - 10000) > msg_id) {
            /* wrap-around case */
            crm_debug("Received old reply: %d (wanted %d)", reply_id, msg_id);
            crm_log_xml_trace(op_reply, "Old reply");
        } else {
            crm_err("Received a __future__ reply:" " %d (wanted %d)", reply_id, msg_id);
        }
 
         free_xml(op_reply);
         op_reply = NULL;
 
        /* wasn't the right reply, try and read some more */
        remaining_time = time(NULL) - start_time;
     }
 
     /* if(IPC_ISRCONN(native->command_channel) == FALSE) { */
    /*      crm_err("CIB disconnected: %d",  */
    /*              native->command_channel->ch_status); */
     /*      cib->state = cib_disconnected; */
     /* } */
 
    if (disconnected) {
        crm_err("Disconnected while waiting for reply.");
        return -ENOTCONN;
    } else if (op_reply == NULL) {
         crm_err("No reply message - empty");
         return -ENOMSG;
     }

    crm_trace("Syncronous reply received");

    /* Start processing the reply... */
    if (crm_element_value_int(op_reply, F_CIB_RC, &rc) != 0) {
        rc = -EPROTO;
    }

    if (rc == -pcmk_err_diff_resync) {
        /* This is an internal value that clients do not and should not care about */
        rc = pcmk_ok;
    }

    if (rc == pcmk_ok || rc == -EPERM) {
        crm_log_xml_debug(op_reply, "passed");

    } else {
/* 	} else if(rc == -ETIME) { */
        crm_err("Call failed: %s", pcmk_strerror(rc));
        crm_log_xml_warn(op_reply, "failed");
    }

    if (output_data == NULL) {
        /* do nothing more */

    } else if (!(call_options & cib_discard_reply)) {
        xmlNode *tmp = get_message_xml(op_reply, F_CIB_CALLDATA);

        if (tmp == NULL) {
            crm_trace("No output in reply to \"%s\" command %d", op, cib->call_id - 1);
        } else {
            *output_data = copy_xml(tmp);
        }
    }

    free_xml(op_reply);

    return rc;
}
