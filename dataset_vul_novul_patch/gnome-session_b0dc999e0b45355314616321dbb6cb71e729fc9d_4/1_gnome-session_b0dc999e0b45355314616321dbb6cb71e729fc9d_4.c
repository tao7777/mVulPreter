accept_xsmp_connection (SmsConn        sms_conn,
                        GsmXsmpServer *server,
                        unsigned long *mask_ret,
                         SmsCallbacks  *callbacks_ret,
                         char         **failure_reason_ret)
 {
        IceConn        ice_conn;
        GsmXSMPClient *client;
 
         /* FIXME: what about during shutdown but before gsm_xsmp_shutdown? */
         if (server->priv->xsmp_sockets == NULL) {
                g_debug ("GsmXsmpServer: In shutdown, rejecting new client");

                *failure_reason_ret = strdup (_("Refusing new client connection because the session is currently being shut down\n"));
                return FALSE;
         }
 
         ice_conn = SmsGetIceConnection (sms_conn);
        client = ice_conn->context;
 
        g_return_val_if_fail (client != NULL, TRUE);
 
        gsm_xsmp_client_connect (client, sms_conn, mask_ret, callbacks_ret);
 
         return TRUE;
 }
