gsm_xsmp_client_connect (GsmXSMPClient *client,
                         SmsConn        conn,
                         unsigned long *mask_ret,
                         SmsCallbacks  *callbacks_ret)
 {
         client->priv->conn = conn;
 
         g_debug ("GsmXSMPClient: Initializing client %s", client->priv->description);
 
         *mask_ret = 0;

        *mask_ret |= SmsRegisterClientProcMask;
        callbacks_ret->register_client.callback = register_client_callback;
        callbacks_ret->register_client.manager_data  = client;

        *mask_ret |= SmsInteractRequestProcMask;
        callbacks_ret->interact_request.callback = interact_request_callback;
        callbacks_ret->interact_request.manager_data = client;

        *mask_ret |= SmsInteractDoneProcMask;
        callbacks_ret->interact_done.callback = interact_done_callback;
        callbacks_ret->interact_done.manager_data = client;

        *mask_ret |= SmsSaveYourselfRequestProcMask;
        callbacks_ret->save_yourself_request.callback = save_yourself_request_callback;
        callbacks_ret->save_yourself_request.manager_data = client;

        *mask_ret |= SmsSaveYourselfP2RequestProcMask;
        callbacks_ret->save_yourself_phase2_request.callback = save_yourself_phase2_request_callback;
        callbacks_ret->save_yourself_phase2_request.manager_data = client;

        *mask_ret |= SmsSaveYourselfDoneProcMask;
        callbacks_ret->save_yourself_done.callback = save_yourself_done_callback;
        callbacks_ret->save_yourself_done.manager_data = client;

        *mask_ret |= SmsCloseConnectionProcMask;
        callbacks_ret->close_connection.callback = close_connection_callback;
        callbacks_ret->close_connection.manager_data  = client;

        *mask_ret |= SmsSetPropertiesProcMask;
        callbacks_ret->set_properties.callback = set_properties_callback;
        callbacks_ret->set_properties.manager_data = client;

        *mask_ret |= SmsDeletePropertiesProcMask;
        callbacks_ret->delete_properties.callback = delete_properties_callback;
        callbacks_ret->delete_properties.manager_data = client;

        *mask_ret |= SmsGetPropertiesProcMask;
        callbacks_ret->get_properties.callback = get_properties_callback;
        callbacks_ret->get_properties.manager_data = client;
}
