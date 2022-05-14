static void parse_class(RBinFile *binfile, RBinDexObj *bin, RBinDexClass *c,
			 int class_index, int *methods, int *sym_count) {
	struct r_bin_t *rbin = binfile->rbin;

	char *class_name;
	int z;
	const ut8 *p, *p_end;

	if (!c) {
		return;
	}

	class_name = dex_class_name (bin, c);
	class_name = r_str_replace (class_name, ";", "", 0); //TODO: move to func

	if (!class_name || !*class_name) {
		return;
	}

	RBinClass *cls = R_NEW0 (RBinClass);
	if (!cls) {
		return;
	}
	cls->name = class_name;
	cls->index = class_index;
	cls->addr = bin->header.class_offset + class_index * DEX_CLASS_SIZE;
	cls->methods = r_list_new ();
	if (!cls->methods) {
		free (cls);
		return;
	}
	cls->fields = r_list_new ();
	if (!cls->fields) {
		r_list_free (cls->methods);
		free (cls);
		return;
	}
	r_list_append (bin->classes_list, cls);
	if (dexdump) {
		rbin->cb_printf ("  Class descriptor  : '%s;'\n", class_name);
		rbin->cb_printf (
			"  Access flags      : 0x%04x (%s)\n", c->access_flags,
			createAccessFlagStr (c->access_flags, kAccessForClass));
		rbin->cb_printf ("  Superclass        : '%s'\n",
				 dex_class_super_name (bin, c));
		rbin->cb_printf ("  Interfaces        -\n");
	}

	if (c->interfaces_offset > 0 &&
	    bin->header.data_offset < c->interfaces_offset &&
 	    c->interfaces_offset <
 		    bin->header.data_offset + bin->header.data_size) {
 		p = r_buf_get_at (binfile->buf, c->interfaces_offset, NULL);
		int types_list_size = r_read_le32(p);
 		if (types_list_size < 0 || types_list_size >= bin->header.types_size ) {
 			return;
 		}
		for (z = 0; z < types_list_size; z++) {
			int t = r_read_le16 (p + 4 + z * 2);
			if (t > 0 && t < bin->header.types_size ) {
				int tid = bin->types[t].descriptor_id;
				if (dexdump) {
					rbin->cb_printf (
						"    #%d              : '%s'\n",
						z, getstr (bin, tid));
				}
			}
		}
	}

	if (!c || !c->class_data_offset) {
		if (dexdump) {
			rbin->cb_printf (
				"  Static fields     -\n  Instance fields   "
				"-\n  Direct methods    -\n  Virtual methods   "
				"-\n");
		}
	} else {
		if (bin->header.class_offset > c->class_data_offset ||
		    c->class_data_offset <
			    bin->header.class_offset +
				    bin->header.class_size * DEX_CLASS_SIZE) {
			return;
		}

		p = r_buf_get_at (binfile->buf, c->class_data_offset, NULL);
		p_end = p + binfile->buf->length - c->class_data_offset;
		c->class_data = (struct dex_class_data_item_t *)malloc (
			sizeof (struct dex_class_data_item_t));
		p = r_uleb128 (p, p_end - p, &c->class_data->static_fields_size);
		p = r_uleb128 (p, p_end - p, &c->class_data->instance_fields_size);
		p = r_uleb128 (p, p_end - p, &c->class_data->direct_methods_size);
		p = r_uleb128 (p, p_end - p, &c->class_data->virtual_methods_size);

		if (dexdump) { 
			rbin->cb_printf ("  Static fields     -\n"); 
		}
		p = parse_dex_class_fields (
			binfile, bin, c, cls, p, p_end, sym_count,
			c->class_data->static_fields_size, true);

		if (dexdump) { 
			rbin->cb_printf ("  Instance fields   -\n");
		}
		p = parse_dex_class_fields (
			binfile, bin, c, cls, p, p_end, sym_count,
			c->class_data->instance_fields_size, false);

		if (dexdump) { 
			rbin->cb_printf ("  Direct methods    -\n");
		}
		p = parse_dex_class_method (
			binfile, bin, c, cls, p, p_end, sym_count,
			c->class_data->direct_methods_size, methods, true);

		if (dexdump) { 
			rbin->cb_printf ("  Virtual methods   -\n");
		}
		p = parse_dex_class_method (
			binfile, bin, c, cls, p, p_end, sym_count,
			c->class_data->virtual_methods_size, methods, false);
	}

	if (dexdump) { 
		char *source_file = getstr (bin, c->source_file);
		if (!source_file) {
			rbin->cb_printf (
				"  source_file_idx   : %d (unknown)\n\n",
				c->source_file);
		} else {
			rbin->cb_printf ("  source_file_idx   : %d (%s)\n\n",
					 c->source_file, source_file);
		}
	}
}
