static void destroy_server_connect(SERVER_CONNECT_REC *conn)
{
	IRC_SERVER_CONNECT_REC *ircconn;

        ircconn = IRC_SERVER_CONNECT(conn);
	if (ircconn == NULL)
		return;
 
 	g_free_not_null(ircconn->usermode);
 	g_free_not_null(ircconn->alternate_nick);
 }
