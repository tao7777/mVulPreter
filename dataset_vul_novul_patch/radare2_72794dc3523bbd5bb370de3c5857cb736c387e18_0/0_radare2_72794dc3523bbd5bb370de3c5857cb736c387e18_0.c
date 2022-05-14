static RList *relocs(RBinFile *arch) {
	struct r_bin_bflt_obj *obj = (struct r_bin_bflt_obj*)arch->o->bin_obj;
	RList *list = r_list_newf ((RListFree)free);
	int i, len, n_got, amount;
	if (!list || !obj) {
		r_list_free (list);
		return NULL;
	}
	if (obj->hdr->flags & FLAT_FLAG_GOTPIC) {
		n_got = get_ngot_entries (obj);
		if (n_got) {
			amount = n_got *  sizeof (ut32);
 			if (amount < n_got || amount > UT32_MAX) {
 				goto out_error;
 			}
			struct reloc_struct_t *got_table = calloc (
				1, n_got * sizeof (struct reloc_struct_t));
 			if (got_table) {
 				ut32 offset = 0;
 				for (i = 0; i < n_got ; offset += 4, i++) {
					ut32 got_entry;
					if (obj->hdr->data_start + offset + 4 > obj->size ||
					    obj->hdr->data_start + offset + 4 < offset) {
						break;
					}
					len = r_buf_read_at (obj->b, obj->hdr->data_start + offset,
								(ut8 *)&got_entry, sizeof (ut32));
					if (!VALID_GOT_ENTRY (got_entry) || len != sizeof (ut32)) {
						break;
					}
					got_table[i].addr_to_patch = got_entry;
					got_table[i].data_offset = got_entry + BFLT_HDR_SIZE;
				}
				obj->n_got = n_got;
				obj->got_table = got_table;
			}
		}
	}

	if (obj->hdr->reloc_count > 0) {
		int n_reloc = obj->hdr->reloc_count; 

		amount = n_reloc * sizeof (struct reloc_struct_t);
		if (amount < n_reloc || amount > UT32_MAX) {
			goto out_error;	
		}
		struct reloc_struct_t *reloc_table = calloc (1, amount + 1);
		if (!reloc_table) {
			goto out_error;
		}
		amount = n_reloc * sizeof (ut32);
		if (amount < n_reloc || amount > UT32_MAX) {
			free (reloc_table);
			goto out_error;
		}
		ut32 *reloc_pointer_table = calloc (1, amount + 1);
		if (!reloc_pointer_table) {
			free (reloc_table);
			goto out_error;
		}
		if (obj->hdr->reloc_start + amount > obj->size ||
		    obj->hdr->reloc_start + amount < amount) {
			free (reloc_table);
			free (reloc_pointer_table);
			goto out_error;
		}
		len = r_buf_read_at (obj->b, obj->hdr->reloc_start,
				     (ut8 *)reloc_pointer_table, amount);
		if (len != amount) {
			free (reloc_table);
			free (reloc_pointer_table);
			goto out_error;
		}
		for (i = 0; i < obj->hdr->reloc_count; i++) {
			ut32 reloc_offset =
				r_swap_ut32 (reloc_pointer_table[i]) +
				BFLT_HDR_SIZE;

			if (reloc_offset < obj->hdr->bss_end && reloc_offset < obj->size) {
				ut32 reloc_fixed, reloc_data_offset;
				if (reloc_offset + sizeof (ut32) > obj->size ||
				    reloc_offset + sizeof (ut32) < reloc_offset) {
					free (reloc_table);
					free (reloc_pointer_table);
					goto out_error;
				}
				len = r_buf_read_at (obj->b, reloc_offset,
						     (ut8 *)&reloc_fixed,
						     sizeof (ut32));
				if (len != sizeof (ut32)) {
					eprintf ("problem while reading relocation entries\n");
					free (reloc_table);
					free (reloc_pointer_table);
					goto out_error;
				}
				reloc_data_offset = r_swap_ut32 (reloc_fixed) + BFLT_HDR_SIZE;
				reloc_table[i].addr_to_patch = reloc_offset;
				reloc_table[i].data_offset = reloc_data_offset;
	
				RBinReloc *reloc = R_NEW0 (RBinReloc);
				if (reloc) {
					reloc->type = R_BIN_RELOC_32;
					reloc->paddr = reloc_table[i].addr_to_patch;
					reloc->vaddr = reloc->paddr;
					r_list_append (list, reloc);
				}
			}
		}
		free (reloc_pointer_table);
		obj->reloc_table = reloc_table;
	}
	return list;
out_error:
	r_list_free (list);
	return NULL;	
}
