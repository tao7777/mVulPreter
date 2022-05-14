static void sig_server_setup_fill_chatnet(IRC_SERVER_CONNECT_REC *conn,
					  IRC_CHATNET_REC *ircnet)
{
	if (!IS_IRC_SERVER_CONNECT(conn))
		return;
	g_return_if_fail(IS_IRCNET(ircnet));

	if (ircnet->alternate_nick != NULL) {
		g_free_and_null(conn->alternate_nick);
		conn->alternate_nick = g_strdup(ircnet->alternate_nick);
	}
	if (ircnet->usermode != NULL) {
		g_free_and_null(conn->usermode);
		conn->usermode = g_strdup(ircnet->usermode);
	}

	if (ircnet->max_kicks > 0) conn->max_kicks = ircnet->max_kicks;
	if (ircnet->max_msgs > 0) conn->max_msgs = ircnet->max_msgs;
	if (ircnet->max_modes > 0) conn->max_modes = ircnet->max_modes;
	if (ircnet->max_whois > 0) conn->max_whois = ircnet->max_whois;

	if (ircnet->max_cmds_at_once > 0)
		conn->max_cmds_at_once = ircnet->max_cmds_at_once;
	if (ircnet->cmd_queue_speed > 0)
		conn->cmd_queue_speed = ircnet->cmd_queue_speed;
	if (ircnet->max_query_chans > 0)
		conn->max_query_chans = ircnet->max_query_chans;

	/* Validate the SASL parameters filled by sig_chatnet_read() or cmd_network_add */
	conn->sasl_mechanism = SASL_MECHANISM_NONE;
	conn->sasl_username = NULL;
	conn->sasl_password = NULL;

	if (ircnet->sasl_mechanism != NULL) {
		if (!g_ascii_strcasecmp(ircnet->sasl_mechanism, "plain")) {
			/* The PLAIN method needs both the username and the password */
 			conn->sasl_mechanism = SASL_MECHANISM_PLAIN;
 			if (ircnet->sasl_username != NULL && *ircnet->sasl_username &&
 			    ircnet->sasl_password != NULL && *ircnet->sasl_password) {
				conn->sasl_username = ircnet->sasl_username;
				conn->sasl_password = ircnet->sasl_password;
 			} else
 				g_warning("The fields sasl_username and sasl_password are either missing or empty");
 		}
		else if (!g_ascii_strcasecmp(ircnet->sasl_mechanism, "external")) {
			conn->sasl_mechanism = SASL_MECHANISM_EXTERNAL;
		}
		else
			g_warning("Unsupported SASL mechanism \"%s\" selected", ircnet->sasl_mechanism);
	}
}
