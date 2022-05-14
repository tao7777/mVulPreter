 static int get_debug_info(struct PE_(r_bin_pe_obj_t)* bin, PE_(image_debug_directory_entry)* dbg_dir_entry, ut8* dbg_data, int dbg_data_len, SDebugInfo* res) {
	#define SIZEOF_FILE_NAME 255
	int i = 0;
	const char* basename;
	if (!dbg_data) {
		return 0;
	}
	switch (dbg_dir_entry->Type) {
	case IMAGE_DEBUG_TYPE_CODEVIEW:
		if (!strncmp ((char*) dbg_data, "RSDS", 4)) {
			SCV_RSDS_HEADER rsds_hdr;
			init_rsdr_hdr (&rsds_hdr);
			if (!get_rsds (dbg_data, dbg_data_len, &rsds_hdr)) {
				bprintf ("Warning: Cannot read PE debug info\n");
				return 0;
			}
			snprintf (res->guidstr, GUIDSTR_LEN,
				"%08x%04x%04x%02x%02x%02x%02x%02x%02x%02x%02x%x",
				rsds_hdr.guid.data1,
				rsds_hdr.guid.data2,
				rsds_hdr.guid.data3,
				rsds_hdr.guid.data4[0],
				rsds_hdr.guid.data4[1],
				rsds_hdr.guid.data4[2],
				rsds_hdr.guid.data4[3],
				rsds_hdr.guid.data4[4],
				rsds_hdr.guid.data4[5],
				rsds_hdr.guid.data4[6],
				rsds_hdr.guid.data4[7],
				rsds_hdr.age);
			basename = r_file_basename ((char*) rsds_hdr.file_name);
			strncpy (res->file_name, (const char*)
				basename, sizeof (res->file_name));
 			res->file_name[sizeof (res->file_name) - 1] = 0;
 			rsds_hdr.free ((struct SCV_RSDS_HEADER*) &rsds_hdr);
 		} else if (strncmp ((const char*) dbg_data, "NB10", 4) == 0) {
			if (dbg_data_len < 20) {
				eprintf ("Truncated NB10 entry, not enough data to parse\n");
				return 0;
			}
			SCV_NB10_HEADER nb10_hdr = {{0}};
 			init_cv_nb10_header (&nb10_hdr);
 			get_nb10 (dbg_data, &nb10_hdr);
 			snprintf (res->guidstr, sizeof (res->guidstr),
 				"%x%x", nb10_hdr.timestamp, nb10_hdr.age);
			res->file_name[0] = 0;
			if (nb10_hdr.file_name) {
				strncpy (res->file_name, (const char*)
						nb10_hdr.file_name, sizeof (res->file_name) - 1);
			}
 			res->file_name[sizeof (res->file_name) - 1] = 0;
 			nb10_hdr.free ((struct SCV_NB10_HEADER*) &nb10_hdr);
 		} else {
			bprintf ("CodeView section not NB10 or RSDS\n");
			return 0;
		}
		break;
	default:
		return 0;
	}

	while (i < 33) {
		res->guidstr[i] = toupper ((int) res->guidstr[i]);
		i++;
	}

	return 1;
}
