static void sig_server_connect_copy(SERVER_CONNECT_REC **dest,
				    IRC_SERVER_CONNECT_REC *src)
{
	IRC_SERVER_CONNECT_REC *rec;

	g_return_if_fail(dest != NULL);
	if (!IS_IRC_SERVER_CONNECT(src))
		return;

	rec = g_new0(IRC_SERVER_CONNECT_REC, 1);
	rec->chat_type = IRC_PROTOCOL;
	rec->max_cmds_at_once = src->max_cmds_at_once;
	rec->cmd_queue_speed = src->cmd_queue_speed;
        rec->max_query_chans = src->max_query_chans;
	rec->max_kicks = src->max_kicks;
	rec->max_modes = src->max_modes;
	rec->max_msgs = src->max_msgs;
	rec->max_whois = src->max_whois;
 	rec->usermode = g_strdup(src->usermode);
 	rec->alternate_nick = g_strdup(src->alternate_nick);
 	rec->sasl_mechanism = src->sasl_mechanism;
	rec->sasl_username = g_strdup(src->sasl_username);
	rec->sasl_password = g_strdup(src->sasl_password);
 	*dest = (SERVER_CONNECT_REC *) rec;
 }
