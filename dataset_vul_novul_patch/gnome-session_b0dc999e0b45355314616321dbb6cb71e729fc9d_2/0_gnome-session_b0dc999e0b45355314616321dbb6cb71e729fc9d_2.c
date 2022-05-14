setup_connection (GsmXSMPClient *client)
{
        GIOChannel    *channel;
        int            fd;

        g_debug ("GsmXSMPClient: Setting up new connection");

        fd = IceConnectionNumber (client->priv->ice_connection);
        fcntl (fd, F_SETFD, fcntl (fd, F_GETFD, 0) | FD_CLOEXEC);
        channel = g_io_channel_unix_new (fd);
        client->priv->watch_id = g_io_add_watch (channel,
                                                 G_IO_IN | G_IO_ERR,
                                                 (GIOFunc)client_iochannel_watch,
                                                  client);
         g_io_channel_unref (channel);
 
         set_description (client);
 
         g_debug ("GsmXSMPClient: New client '%s'", client->priv->description);
}
