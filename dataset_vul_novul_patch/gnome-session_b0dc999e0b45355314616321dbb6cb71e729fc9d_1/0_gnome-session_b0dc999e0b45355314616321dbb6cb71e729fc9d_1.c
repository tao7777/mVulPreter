gsm_xsmp_client_disconnect (GsmXSMPClient *client)
{
        if (client->priv->watch_id > 0) {
                g_source_remove (client->priv->watch_id);
        }

        if (client->priv->conn != NULL) {
                SmsCleanUp (client->priv->conn);
        }

        if (client->priv->ice_connection != NULL) {
                 IceSetShutdownNegotiation (client->priv->ice_connection, FALSE);
                 IceCloseConnection (client->priv->ice_connection);
         }
 }
