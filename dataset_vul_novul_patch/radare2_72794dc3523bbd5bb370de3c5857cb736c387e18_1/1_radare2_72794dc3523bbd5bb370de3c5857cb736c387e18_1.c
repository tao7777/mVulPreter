static int search_old_relocation(struct reloc_struct_t *reloc_table, ut32 addr_to_patch, int n_reloc) {
 	int i;
 	for (i = 0; i < n_reloc; i++) {
 		if (addr_to_patch == reloc_table[i].data_offset) {
			return i;
		}
	}
	return -1;
}
