static const ut8 *r_bin_dwarf_parse_comp_unit(Sdb *s, const ut8 *obuf,
		RBinDwarfCompUnit *cu, const RBinDwarfDebugAbbrev *da,
		size_t offset, const ut8 *debug_str, size_t debug_str_len) {
	const ut8 *buf = obuf, *buf_end = obuf + (cu->hdr.length - 7);
	ut64 abbr_code;
	size_t i;
	
	if (cu->hdr.length > debug_str_len) {
		return NULL;
	}
	while (buf && buf < buf_end && buf >= obuf) {
		if (cu->length && cu->capacity == cu->length) {
			r_bin_dwarf_expand_cu (cu);
		}
		buf = r_uleb128 (buf, buf_end - buf, &abbr_code);
		if (abbr_code > da->length || !buf) {
			return NULL;
		}

		r_bin_dwarf_init_die (&cu->dies[cu->length]);
		if (!abbr_code) {
			cu->dies[cu->length].abbrev_code = 0;
			cu->length++;
			buf++;
			continue;
		}

		cu->dies[cu->length].abbrev_code = abbr_code;
		cu->dies[cu->length].tag = da->decls[abbr_code - 1].tag;
		abbr_code += offset;

		if (da->capacity < abbr_code) {
			return NULL;
		}

		for (i = 0; i < da->decls[abbr_code - 1].length; i++) {
			if (cu->dies[cu->length].length == cu->dies[cu->length].capacity) {
				r_bin_dwarf_expand_die (&cu->dies[cu->length]);
			}
			if (i >= cu->dies[cu->length].capacity || i >= da->decls[abbr_code - 1].capacity) {
				eprintf ("Warning: malformed dwarf attribute capacity doesn't match length\n");
				break;
			}
			memset (&cu->dies[cu->length].attr_values[i], 0, sizeof (cu->dies[cu->length].attr_values[i]));
			buf = r_bin_dwarf_parse_attr_value (buf, buf_end - buf,
					&da->decls[abbr_code - 1].specs[i],
					&cu->dies[cu->length].attr_values[i],
 					&cu->hdr, debug_str, debug_str_len);
 			if (cu->dies[cu->length].attr_values[i].name == DW_AT_comp_dir) {
 				const char *name = cu->dies[cu->length].attr_values[i].encoding.str_struct.string;
				if (name > 1024) { // solve some null derefs
					sdb_set (s, "DW_AT_comp_dir", name, 0);
				} else {
					eprintf ("Invalid string pointer at %p\n", name);
				}
 			}
 			cu->dies[cu->length].length++;
 		}
		cu->length++;
	}
	return buf;
}
