static int bin_entry(RCore *r, int mode, ut64 laddr, int va, bool inifin) {
	char str[R_FLAG_NAME_SIZE];
	RList *entries = r_bin_get_entries (r->bin);
	RListIter *iter;
	RBinAddr *entry = NULL;
	int i = 0;
	ut64 baddr = r_bin_get_baddr (r->bin);

	if (IS_MODE_RAD (mode)) {
		r_cons_printf ("fs symbols\n");
	} else if (IS_MODE_JSON (mode)) {
		r_cons_printf ("[");
	} else if (IS_MODE_NORMAL (mode)) {
		if (inifin) {
			r_cons_printf ("[Constructors]\n");
		} else {
 			r_cons_printf ("[Entrypoints]\n");
 		}
 	}
	if (r_list_length (entries) > 1024) {
		eprintf ("Too many entrypoints (%d)\n", r_list_length (entries));
		return false;
	}
 
 	r_list_foreach (entries, iter, entry) {
 		ut64 paddr = entry->paddr;
		ut64 haddr = UT64_MAX;
		if (mode != R_CORE_BIN_SET) {
			if (inifin) {
				if (entry->type == R_BIN_ENTRY_TYPE_PROGRAM) {
					continue;
				}
			} else {
				if (entry->type != R_BIN_ENTRY_TYPE_PROGRAM) {
					continue;
				}
			}
		}
		switch (entry->type) {
		case R_BIN_ENTRY_TYPE_INIT:
		case R_BIN_ENTRY_TYPE_FINI:
		case R_BIN_ENTRY_TYPE_PREINIT:
			if (r->io->va && entry->paddr == entry->vaddr) {
				RIOMap *map = r_io_map_get (r->io, entry->vaddr);
				if (map) {
					paddr = entry->vaddr - map->itv.addr + map->delta;
				}
			}
		}
		if (entry->haddr) {
			haddr = entry->haddr;
		}
		ut64 at = rva (r->bin, paddr, entry->vaddr, va);
		const char *type = r_bin_entry_type_string (entry->type);
		if (!type) {
			type = "unknown";
		}
		if (IS_MODE_SET (mode)) {
			r_flag_space_set (r->flags, "symbols");
			if (entry->type == R_BIN_ENTRY_TYPE_INIT) {
				snprintf (str, R_FLAG_NAME_SIZE, "entry%i.init", i);
			} else if (entry->type == R_BIN_ENTRY_TYPE_FINI) {
				snprintf (str, R_FLAG_NAME_SIZE, "entry%i.fini", i);
			} else if (entry->type == R_BIN_ENTRY_TYPE_PREINIT) {
				snprintf (str, R_FLAG_NAME_SIZE, "entry%i.preinit", i);
			} else {
				snprintf (str, R_FLAG_NAME_SIZE, "entry%i", i);
			}
			r_flag_set (r->flags, str, at, 1);
		} else if (IS_MODE_SIMPLE (mode)) {
			r_cons_printf ("0x%08"PFMT64x"\n", at);
		} else if (IS_MODE_JSON (mode)) {
			r_cons_printf ("%s{\"vaddr\":%" PFMT64d ","
				"\"paddr\":%" PFMT64d ","
				"\"baddr\":%" PFMT64d ","
				"\"laddr\":%" PFMT64d ","
				"\"haddr\":%" PFMT64d ","
				"\"type\":\"%s\"}",
				iter->p ? "," : "", at, paddr, baddr, laddr, haddr, type);
		} else if (IS_MODE_RAD (mode)) {
			char *name = NULL;
			if (entry->type == R_BIN_ENTRY_TYPE_INIT) {
				name = r_str_newf ("entry%i.init", i);
			} else if (entry->type == R_BIN_ENTRY_TYPE_FINI) {
				name = r_str_newf ("entry%i.fini", i);
			} else if (entry->type == R_BIN_ENTRY_TYPE_PREINIT) {
				name = r_str_newf ("entry%i.preinit", i);
			} else {
				name = r_str_newf ("entry%i", i);
			}
			r_cons_printf ("f %s 1 @ 0x%08"PFMT64x"\n", name, at);
			r_cons_printf ("f %s_haddr 1 @ 0x%08"PFMT64x"\n", name, haddr);
			r_cons_printf ("s %s\n", name);
			free (name);
		} else {
			r_cons_printf (
				 "vaddr=0x%08"PFMT64x
				" paddr=0x%08"PFMT64x
				" baddr=0x%08"PFMT64x
				" laddr=0x%08"PFMT64x,
				at, paddr, baddr, laddr);
			if (haddr == UT64_MAX) {
				r_cons_printf (
					" haddr=%"PFMT64d
					" type=%s\n",
					haddr, type);
			} else {
				r_cons_printf (
					" haddr=0x%08"PFMT64x
					" type=%s\n",
					haddr, type);
			}
		}
		i++;
	}
	if (IS_MODE_SET (mode)) {
		if (entry) {
			ut64 at = rva (r->bin, entry->paddr, entry->vaddr, va);
			r_core_seek (r, at, 0);
		}
	} else if (IS_MODE_JSON (mode)) {
		r_cons_printf ("]");
		r_cons_newline ();
	} else if (IS_MODE_NORMAL (mode)) {
		r_cons_printf ("\n%i entrypoints\n", i);
	}
	return true;
}
