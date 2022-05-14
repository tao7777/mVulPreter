static void add_probe(const char *name)
 {
 	struct module_entry *m;
 
 	m = get_or_add_modentry(name);
 	if (!(option_mask32 & (OPT_REMOVE | OPT_SHOW_DEPS))
 	 && (m->flags & MODULE_FLAG_LOADED)
	 && strncmp(m->modname, "symbol:", 7) == 0
	) {
		G.need_symbols = 1;
	}
}
