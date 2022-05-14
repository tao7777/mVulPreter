struct symbol_t* MACH0_(get_symbols)(struct MACH0_(obj_t)* bin) {
	const char *symstr;
	struct symbol_t *symbols;
	int from, to, i, j, s, stridx, symbols_size, symbols_count;
	SdbHash *hash;

	if (!bin || !bin->symtab || !bin->symstr) {
		return NULL;
	}
	/* parse symbol table */
	/* parse dynamic symbol table */
	symbols_count = (bin->dysymtab.nextdefsym + \
			bin->dysymtab.nlocalsym + \
			bin->dysymtab.nundefsym );
	symbols_count += bin->nsymtab;
	symbols_size = (symbols_count + 1) * 2 * sizeof (struct symbol_t);

	if (symbols_size < 1) {
		return NULL;
	}
	if (!(symbols = calloc (1, symbols_size))) {
		return NULL;
	}
	hash = sdb_ht_new ();
	j = 0; // symbol_idx
	for (s = 0; s < 2; s++) {
		switch (s) {
		case 0:
			from = bin->dysymtab.iextdefsym;
			to = from + bin->dysymtab.nextdefsym;
			break;
		case 1:
			from = bin->dysymtab.ilocalsym;
			to = from + bin->dysymtab.nlocalsym;
			break;
#if NOT_USED
		case 2:
			from = bin->dysymtab.iundefsym;
			to = from + bin->dysymtab.nundefsym;
			break;
#endif
		}
		if (from == to) {
			continue;
		}
#define OLD 1
#if OLD
		from = R_MIN (R_MAX (0, from), symbols_size / sizeof (struct symbol_t));
		to = R_MIN (to , symbols_size / sizeof (struct symbol_t));
		to = R_MIN (to, bin->nsymtab);
#else
		from = R_MIN (R_MAX (0, from), symbols_size/sizeof (struct symbol_t));
		to = symbols_count; //symbols_size/sizeof(struct symbol_t);
#endif
		int maxsymbols = symbols_size / sizeof (struct symbol_t);
		if (to > 0x500000) {
			bprintf ("WARNING: corrupted mach0 header: symbol table is too big %d\n", to);
			free (symbols);
			sdb_ht_free (hash);
			return NULL;
		}
		if (symbols_count >= maxsymbols) {
			symbols_count = maxsymbols - 1;
		}
		for (i = from; i < to && j < symbols_count; i++, j++) {
			symbols[j].offset = addr_to_offset (bin, bin->symtab[i].n_value);
			symbols[j].addr = bin->symtab[i].n_value;
			symbols[j].size = 0; /* TODO: Is it anywhere? */
			if (bin->symtab[i].n_type & N_EXT) {
				symbols[j].type = R_BIN_MACH0_SYMBOL_TYPE_EXT;
			} else {
				symbols[j].type = R_BIN_MACH0_SYMBOL_TYPE_LOCAL;
			}
			stridx = bin->symtab[i].n_strx;
			if (stridx >= 0 && stridx < bin->symstrlen) {
				symstr = (char*)bin->symstr + stridx;
			} else {
				symstr = "???";
			}
			{
				int i = 0;
				int len = 0;
				len = bin->symstrlen - stridx;
				if (len > 0) {
					for (i = 0; i < len; i++) {
						if ((ut8)(symstr[i] & 0xff) == 0xff || !symstr[i]) {
							len = i;
							break;
						}
					}
					char *symstr_dup = NULL;
					if (len > 0) {
						symstr_dup = r_str_ndup (symstr, len);
					}
					if (!symstr_dup) {
						symbols[j].name[0] = 0;
					} else {
						r_str_ncpy (symbols[j].name, symstr_dup, R_BIN_MACH0_STRING_LENGTH);
						r_str_filter (symbols[j].name, -1);
						symbols[j].name[R_BIN_MACH0_STRING_LENGTH - 2] = 0;
					}
					free (symstr_dup);
				} else {
					symbols[j].name[0] = 0;
				}
				symbols[j].last = 0;
			}
			if (inSymtab (hash, symbols, symbols[j].name, symbols[j].addr)) {
				symbols[j].name[0] = 0;
				j--;
			}
		}
	}
	to = R_MIN (bin->nsymtab, bin->dysymtab.iundefsym + bin->dysymtab.nundefsym);
	for (i = bin->dysymtab.iundefsym; i < to; i++) {
		if (j > symbols_count) {
 			bprintf ("mach0-get-symbols: error\n");
 			break;
 		}
		if (parse_import_stub(bin, &symbols[j], i)) {
 			symbols[j++].last = 0;
		}
 	}
 
 #if 1
	for (i = 0; i < bin->nsymtab; i++) {
		struct MACH0_(nlist) *st = &bin->symtab[i];
#if 0
		bprintf ("stridx %d -> section %d type %d value = %d\n",
			st->n_strx, st->n_sect, st->n_type, st->n_value);
#endif
		stridx = st->n_strx;
		if (stridx >= 0 && stridx < bin->symstrlen) {
			symstr = (char*)bin->symstr + stridx;
		} else {
			symstr = "???";
		}
		int section = st->n_sect;
		if (section == 1 && j < symbols_count) { // text ??st->n_type == 1)
			/* is symbol */
			symbols[j].addr = st->n_value; // + text_base;
			symbols[j].offset = addr_to_offset (bin, symbols[j].addr);
			symbols[j].size = 0; /* find next symbol and crop */
			if (st->n_type & N_EXT) {
				symbols[j].type = R_BIN_MACH0_SYMBOL_TYPE_EXT;
			} else {
				symbols[j].type = R_BIN_MACH0_SYMBOL_TYPE_LOCAL;
			}
			strncpy (symbols[j].name, symstr, R_BIN_MACH0_STRING_LENGTH);
			symbols[j].name[R_BIN_MACH0_STRING_LENGTH - 1] = 0;
			symbols[j].last = 0;
			if (inSymtab (hash, symbols, symbols[j].name, symbols[j].addr)) {
				symbols[j].name[0] = 0;
			} else {
				j++;
			}
		}
	}
#endif
	sdb_ht_free (hash);
	symbols[j].last = 1;
	return symbols;
}
