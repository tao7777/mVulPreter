 static bool r_bin_mdmp_init_directory(struct r_bin_mdmp_obj *obj) {
 	int i;
	struct minidump_directory entry;
 
 	sdb_num_set (obj->kv, "mdmp_directory.offset",
 			obj->hdr->stream_directory_rva, 0);
	sdb_set (obj->kv, "mdmp_directory.format", "[4]E? "
			"(mdmp_stream_type)StreamType "
 			"(mdmp_location_descriptor)Location", 0);
 
 	/* Parse each entry in the directory */
	ut64 rvadir = obj->hdr->stream_directory_rva;
 	for (i = 0; i < (int)obj->hdr->number_of_streams; i++) {
		ut32 delta = i * sizeof (struct minidump_directory);
		int r = r_buf_read_at (obj->b, rvadir + delta, (ut8*) &entry, sizeof (struct minidump_directory));
		if (r) {
			r_bin_mdmp_init_directory_entry (obj, &entry);
		}
 	}
 
 	return true;
}
