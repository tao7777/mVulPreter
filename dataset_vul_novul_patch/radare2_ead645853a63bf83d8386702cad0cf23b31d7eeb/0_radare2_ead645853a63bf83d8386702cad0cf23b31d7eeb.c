static int dex_loadcode(RBinFile *arch, RBinDexObj *bin) {
	struct r_bin_t *rbin = arch->rbin;
	int i;
	int *methods = NULL;
	int sym_count = 0;

	if (!bin || bin->methods_list) {
		return false;
	}
	bin->code_from = UT64_MAX;
	bin->code_to = 0;
	bin->methods_list = r_list_newf ((RListFree)free);
	if (!bin->methods_list) {
		return false;
	}
	bin->imports_list = r_list_newf ((RListFree)free);
	if (!bin->imports_list) {
		r_list_free (bin->methods_list);
		return false;
	}
	bin->classes_list = r_list_newf ((RListFree)__r_bin_class_free);
	if (!bin->classes_list) {
		r_list_free (bin->methods_list);
		r_list_free (bin->imports_list);
		return false;
	}

	if (bin->header.method_size>bin->size) {
		bin->header.method_size = 0;
		return false;
	}

	/* WrapDown the header sizes to avoid huge allocations */
	bin->header.method_size = R_MIN (bin->header.method_size, bin->size);
	bin->header.class_size = R_MIN (bin->header.class_size, bin->size);
	bin->header.strings_size = R_MIN (bin->header.strings_size, bin->size);

	if (bin->header.strings_size > bin->size) {
		eprintf ("Invalid strings size\n");
		return false;
	}

	if (bin->classes) {
		ut64 amount = sizeof (int) * bin->header.method_size;
		if (amount > UT32_MAX || amount < bin->header.method_size) {
			return false;
		}
		methods = calloc (1, amount + 1);
		for (i = 0; i < bin->header.class_size; i++) {
			char *super_name, *class_name;
			struct dex_class_t *c = &bin->classes[i];
			class_name = dex_class_name (bin, c);
			super_name = dex_class_super_name (bin, c);
			if (dexdump) { 
				rbin->cb_printf ("Class #%d            -\n", i);
			}
			parse_class (arch, bin, c, i, methods, &sym_count);
			free (class_name);
			free (super_name);
		}
	}

	if (methods) {
		int import_count = 0;
		int sym_count = bin->methods_list->length;

		for (i = 0; i < bin->header.method_size; i++) {
			int len = 0;
			if (methods[i]) {
 				continue;
 			}
 
			if (bin->methods[i].class_id > bin->header.types_size) {
 				continue;
 			}
 
			if (is_class_idx_in_code_classes(bin, bin->methods[i].class_id)) {
				continue;
			}

			char *class_name = getstr (
				bin, bin->types[bin->methods[i].class_id]
						.descriptor_id);
			if (!class_name) {
				free (class_name);
				continue;
			}
			len = strlen (class_name);
			if (len < 1) {
				continue;
			}
			class_name[len - 1] = 0; // remove last char ";"
			char *method_name = dex_method_name (bin, i);
			char *signature = dex_method_signature (bin, i);
			if (method_name && *method_name) {
				RBinImport *imp = R_NEW0 (RBinImport);
				imp->name  = r_str_newf ("%s.method.%s%s", class_name, method_name, signature);
				imp->type = r_str_const ("FUNC");
				imp->bind = r_str_const ("NONE");
				imp->ordinal = import_count++;
				r_list_append (bin->imports_list, imp);

				RBinSymbol *sym = R_NEW0 (RBinSymbol);
				sym->name = r_str_newf ("imp.%s", imp->name);
				sym->type = r_str_const ("FUNC");
				sym->bind = r_str_const ("NONE");
				sym->paddr = sym->vaddr = bin->b->base + bin->header.method_offset + (sizeof (struct dex_method_t) * i) ;
				sym->ordinal = sym_count++;
				r_list_append (bin->methods_list, sym);
				sdb_num_set (mdb, sdb_fmt (0, "method.%d", i), sym->paddr, 0);
			}
			free (method_name);
			free (signature);
			free (class_name);
		}
		free (methods);
	}
	return true;
}
