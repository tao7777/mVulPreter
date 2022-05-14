static void process_constructors (RBinFile *bf, RList *ret, int bits) {
	RList *secs = sections (bf);
	RListIter *iter;
	RBinSection *sec;
	int i, type;
	r_list_foreach (secs, iter, sec) {
		type = -1;
		if (!strcmp (sec->name, ".fini_array")) {
			type = R_BIN_ENTRY_TYPE_FINI;
		} else if (!strcmp (sec->name, ".init_array")) {
			type = R_BIN_ENTRY_TYPE_INIT;
		} else if (!strcmp (sec->name, ".preinit_array")) {
			type = R_BIN_ENTRY_TYPE_PREINIT;
		}
		if (type != -1) {
			ut8 *buf = calloc (sec->size, 1);
			if (!buf) {
				continue;
 			}
 			(void)r_buf_read_at (bf->buf, sec->paddr, buf, sec->size);
 			if (bits == 32) {
				for (i = 0; i < sec->size; i += 4) {
 					ut32 addr32 = r_read_le32 (buf + i);
 					if (addr32) {
 						RBinAddr *ba = newEntry (sec->paddr + i, (ut64)addr32, type, bits);
 						r_list_append (ret, ba);
 					}
 				}
 			} else {
				for (i = 0; i < sec->size; i += 8) {
 					ut64 addr64 = r_read_le64 (buf + i);
 					if (addr64) {
 						RBinAddr *ba = newEntry (sec->paddr + i, addr64, type, bits);
						r_list_append (ret, ba);
					}
				}
			}
			free (buf);
		}
	}
	r_list_free (secs);
}
