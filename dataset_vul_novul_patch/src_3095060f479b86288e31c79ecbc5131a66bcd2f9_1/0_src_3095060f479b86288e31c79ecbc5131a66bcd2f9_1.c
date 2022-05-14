 monitor_init(void)
 {
 	struct monitor *mon;
 
 	mon = xcalloc(1, sizeof(*mon));
 	monitor_openfds(mon, 1);
 
 	return mon;
 }
