static const ut8 *r_bin_dwarf_parse_attr_value(const ut8 *obuf, int obuf_len,
		RBinDwarfAttrSpec *spec, RBinDwarfAttrValue *value,
		const RBinDwarfCompUnitHdr *hdr,
		const ut8 *debug_str, size_t debug_str_len) {
	const ut8 *buf = obuf;
 	const ut8 *buf_end = obuf + obuf_len;
 	size_t j;
 
	if (!spec || !value || !hdr || !obuf || obuf_len < 0) {
 		return NULL;
 	}
 
	value->form = spec->attr_form;
	value->name = spec->attr_name;
	value->encoding.block.data = NULL;
	value->encoding.str_struct.string = NULL;
	value->encoding.str_struct.offset = 0;

	switch (spec->attr_form) {
	case DW_FORM_addr:
		switch (hdr->pointer_size) {
		case 1:
			value->encoding.address = READ (buf, ut8);
			break;
		case 2:
			value->encoding.address = READ (buf, ut16);
			break;
		case 4:
			value->encoding.address = READ (buf, ut32);
			break;
		case 8:
 			value->encoding.address = READ (buf, ut64);
 			break;
 		default:
			eprintf("DWARF: Unexpected pointer size: %u\n", (unsigned)hdr->pointer_size);
 			return NULL;
 		}
 		break;
	case DW_FORM_block2:
		value->encoding.block.length = READ (buf, ut16);
		if (value->encoding.block.length > 0) {
			value->encoding.block.data = calloc (sizeof(ut8), value->encoding.block.length);
			for (j = 0; j < value->encoding.block.length; j++) {
				value->encoding.block.data[j] = READ (buf, ut8);
			}
		}
		break;
	case DW_FORM_block4:
		value->encoding.block.length = READ (buf, ut32);
		if (value->encoding.block.length > 0) {
			ut8 *data = calloc (sizeof (ut8), value->encoding.block.length);
			if (data) {
				for (j = 0; j < value->encoding.block.length; j++) {
					data[j] = READ (buf, ut8);
				}
			}
 			value->encoding.block.data = data;
 		}
 		break;
//// This causes segfaults to happen
 	case DW_FORM_data2:
 		value->encoding.data = READ (buf, ut16);
 		break;
	case DW_FORM_data4:
		value->encoding.data = READ (buf, ut32);
		break;
 	case DW_FORM_data8:
 		value->encoding.data = READ (buf, ut64);
 		break;
 	case DW_FORM_string:
 		value->encoding.str_struct.string = *buf? strdup ((const char*)buf) : NULL;
 		buf += (strlen ((const char*)buf) + 1);
		break;
	case DW_FORM_block:
		buf = r_uleb128 (buf, buf_end - buf, &value->encoding.block.length);
 		if (!buf) {
 			return NULL;
 		}
		value->encoding.block.data = calloc (sizeof(ut8), value->encoding.block.length);
		for (j = 0; j < value->encoding.block.length; j++) {
			value->encoding.block.data[j] = READ (buf, ut8);
 		}
 		break;
 	case DW_FORM_block1:
 		value->encoding.block.length = READ (buf, ut8);
 		value->encoding.block.data = calloc (sizeof (ut8), value->encoding.block.length + 1);
		for (j = 0; j < value->encoding.block.length; j++) {
			value->encoding.block.data[j] = READ (buf, ut8);
 		}
 		break;
 	case DW_FORM_flag:
		value->encoding.flag = READ (buf, ut8);
		break;
	case DW_FORM_sdata:
		buf = r_leb128 (buf, &value->encoding.sdata);
		break;
	case DW_FORM_strp:
		value->encoding.str_struct.offset = READ (buf, ut32);
		if (debug_str && value->encoding.str_struct.offset < debug_str_len) {
			value->encoding.str_struct.string = strdup (
				(const char *)(debug_str +
					value->encoding.str_struct.offset));
		} else {
			value->encoding.str_struct.string = NULL;
		}
		break;
	case DW_FORM_udata:
		{
			ut64 ndata = 0;
			const ut8 *data = (const ut8*)&ndata;
			buf = r_uleb128 (buf, R_MIN (sizeof (data), (size_t)(buf_end - buf)), &ndata);
			memcpy (&value->encoding.data, data, sizeof (value->encoding.data));
			value->encoding.str_struct.string = NULL;
		}
		break;
	case DW_FORM_ref_addr:
		value->encoding.reference = READ (buf, ut64); // addr size of machine
		break;
	case DW_FORM_ref1:
		value->encoding.reference = READ (buf, ut8);
		break;
	case DW_FORM_ref2:
		value->encoding.reference = READ (buf, ut16);
		break;
	case DW_FORM_ref4:
		value->encoding.reference = READ (buf, ut32);
		break;
	case DW_FORM_ref8:
		value->encoding.reference = READ (buf, ut64);
		break;
	case DW_FORM_data1:
		value->encoding.data = READ (buf, ut8);
		break;
	default:
		eprintf ("Unknown DW_FORM 0x%02"PFMT64x"\n", spec->attr_form);
		value->encoding.data = 0;
		return NULL;
	}
	return buf;
}
