 accept_ice_connection (GIOChannel           *source,
                        GIOCondition          condition,
                        GsmIceConnectionData *data)
 {
         IceConn         ice_conn;
         IceAcceptStatus status;
 
         g_debug ("GsmXsmpServer: accept_ice_connection()");
 
        ice_conn = IceAcceptConnection (data->listener, &status);
         if (status != IceAcceptSuccess) {
                 g_debug ("GsmXsmpServer: IceAcceptConnection returned %d", status);
                 return TRUE;
         }
 
        auth_ice_connection (ice_conn);
 
         return TRUE;
 }
