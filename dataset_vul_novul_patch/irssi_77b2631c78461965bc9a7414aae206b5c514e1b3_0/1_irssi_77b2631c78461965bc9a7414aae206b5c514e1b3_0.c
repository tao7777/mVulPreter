 void fe_netjoin_init(void)
 {
 	settings_add_bool("misc", "hide_netsplit_quits", TRUE);
	settings_add_int("misc", "netjoin_max_nicks", 10);

	join_tag = -1;
	printing_joins = FALSE;
 
 	read_settings();
 	signal_add("setup changed", (SIGNAL_FUNC) read_settings);
 }
