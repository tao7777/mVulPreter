static void r_bin_dwarf_dump_debug_info(FILE *f, const RBinDwarfDebugInfo *inf) {
	size_t i, j, k;
	RBinDwarfDIE *dies;
	RBinDwarfAttrValue *values;
	if (!inf || !f) {
		return;
	}

	for (i = 0; i < inf->length; i++) {
		fprintf (f, "  Compilation Unit @ offset 0x%"PFMT64x":\n", inf->comp_units [i].offset);
		fprintf (f, "   Length:        0x%x\n", inf->comp_units [i].hdr.length);
		fprintf (f, "   Version:       %d\n", inf->comp_units [i].hdr.version);
		fprintf (f, "   Abbrev Offset: 0x%x\n", inf->comp_units [i].hdr.abbrev_offset);
		fprintf (f, "   Pointer Size:  %d\n", inf->comp_units [i].hdr.pointer_size);

		dies = inf->comp_units[i].dies;

		for (j = 0; j < inf->comp_units[i].length; j++) {
			fprintf (f, "    Abbrev Number: %"PFMT64u" ", dies[j].abbrev_code);

			if (dies[j].tag && dies[j].tag <= DW_TAG_volatile_type &&
				       dwarf_tag_name_encodings[dies[j].tag]) {
				fprintf (f, "(%s)\n", dwarf_tag_name_encodings[dies[j].tag]);
			} else {
				fprintf (f, "(Unknown abbrev tag)\n");
			}

			if (!dies[j].abbrev_code) {
				continue;
			}
 			values = dies[j].attr_values;
 
 			for (k = 0; k < dies[j].length; k++) {
				if (!values[k].name) {
 					continue;
				}
 
 				if (values[k].name < DW_AT_vtable_elem_location &&
 						dwarf_attr_encodings[values[k].name]) {
					fprintf (f, "     %-18s : ", dwarf_attr_encodings[values[k].name]);
				} else {
					fprintf (f, "     TODO\t");
				}
				r_bin_dwarf_dump_attr_value (&values[k], f);
				fprintf (f, "\n");
			}
		}
	}
}
