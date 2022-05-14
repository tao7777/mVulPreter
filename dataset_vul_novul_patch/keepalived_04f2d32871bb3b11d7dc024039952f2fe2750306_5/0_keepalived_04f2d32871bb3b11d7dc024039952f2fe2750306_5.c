 vrrp_print_data(void)
 {
	FILE *file = fopen_safe(dump_file, "w");
 
 	if (!file) {
 		log_message(LOG_INFO, "Can't open %s (%d: %s)",
			dump_file, errno, strerror(errno));
		return;
	}

	dump_data_vrrp(file);

	fclose(file);
}
