static void add_probe(const char *name)
 {
 	struct module_entry *m;
 
	/*
	 * get_or_add_modentry() strips path from name and works
	 * on remaining basename.
	 * This would make "rmmod dir/name" and "modprobe dir/name"
	 * to work like "rmmod name" and "modprobe name",
	 * which is wrong, and can be abused via implicit modprobing:
	 * "ifconfig /usbserial up" tries to modprobe netdev-/usbserial.
	 */
	if (strchr(name, '/'))
		bb_error_msg_and_die("malformed module name '%s'", name);

 	m = get_or_add_modentry(name);
 	if (!(option_mask32 & (OPT_REMOVE | OPT_SHOW_DEPS))
 	 && (m->flags & MODULE_FLAG_LOADED)
	 && strncmp(m->modname, "symbol:", 7) == 0
	) {
		G.need_symbols = 1;
	}
}
