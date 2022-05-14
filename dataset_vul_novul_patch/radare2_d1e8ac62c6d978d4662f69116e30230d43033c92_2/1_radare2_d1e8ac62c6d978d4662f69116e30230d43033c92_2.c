ut64 MACH0_(get_main)(struct MACH0_(obj_t)* bin) {
	ut64 addr = 0LL;
	struct symbol_t *symbols;
	int i;

	if (!(symbols = MACH0_(get_symbols) (bin))) {
		return 0;
	}
	for (i = 0; !symbols[i].last; i++) {
		if (!strcmp (symbols[i].name, "_main")) {
			addr = symbols[i].addr;
			break;
		}
	}
	free (symbols);

	if (!addr && bin->main_cmd.cmd == LC_MAIN) {
		addr = bin->entry + bin->baddr;
	}

	if (!addr) {
 		ut8 b[128];
 		ut64 entry = addr_to_offset(bin, bin->entry);
		if (entry > bin->size || entry + sizeof (b) > bin->size)
 			return 0;
 		i = r_buf_read_at (bin->b, entry, b, sizeof (b));
 		if (i < 1) {
 			return 0;
		}
		for (i = 0; i < 64; i++) {
			if (b[i] == 0xe8 && !b[i+3] && !b[i+4]) {
				int delta = b[i+1] | (b[i+2] << 8) | (b[i+3] << 16) | (b[i+4] << 24);
				return bin->entry + i + 5 + delta;

			}
		}
	}
	return addr;
}
