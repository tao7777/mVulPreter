 void fe_netjoin_deinit(void)
{
	while (joinservers != NULL)
		netjoin_server_remove(joinservers->data);
	if (join_tag != -1) {
		g_source_remove(join_tag);
		signal_remove("print starting", (SIGNAL_FUNC) sig_print_starting);
 	}
 
 	signal_remove("setup changed", (SIGNAL_FUNC) read_settings);
 
 	signal_remove("message quit", (SIGNAL_FUNC) msg_quit);
 	signal_remove("message join", (SIGNAL_FUNC) msg_join);
	signal_remove("message irc mode", (SIGNAL_FUNC) msg_mode);
}
