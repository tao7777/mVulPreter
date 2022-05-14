static int parse_import_ptr(struct MACH0_(obj_t)* bin, struct reloc_t *reloc, int idx) {
	int i, j, sym, wordsize;
	ut32 stype;
	wordsize = MACH0_(get_bits)(bin) / 8;
	if (idx < 0 || idx >= bin->nsymtab) {
		return 0;
	}
	if ((bin->symtab[idx].n_desc & REFERENCE_TYPE) == REFERENCE_FLAG_UNDEFINED_LAZY) {
		stype = S_LAZY_SYMBOL_POINTERS;
	} else {
		stype = S_NON_LAZY_SYMBOL_POINTERS;
	}

	reloc->offset = 0;
	reloc->addr = 0;
	reloc->addend = 0;
#define CASE(T) case (T / 8): reloc->type = R_BIN_RELOC_ ## T; break
	switch (wordsize) {
		CASE(8);
		CASE(16);
		CASE(32);
		CASE(64);
		default: return false;
	}
#undef CASE
 
 	for (i = 0; i < bin->nsects; i++) {
 		if ((bin->sects[i].flags & SECTION_TYPE) == stype) {
			for (j = 0, sym = -1; bin->sects[i].reserved1 + j < bin->nindirectsyms; j++) {
				int indidx = bin->sects[i].reserved1 + j;
				if (indidx < 0 || indidx >= bin->nindirectsyms) {
					break;
				}
				if (idx == bin->indirectsyms[indidx]) {
 					sym = j;
 					break;
 				}
			}
 			reloc->offset = sym == -1 ? 0 : bin->sects[i].offset + sym * wordsize;
 			reloc->addr = sym == -1 ? 0 : bin->sects[i].addr + sym * wordsize;
 			return true;
		}
	}
	return false;
}
