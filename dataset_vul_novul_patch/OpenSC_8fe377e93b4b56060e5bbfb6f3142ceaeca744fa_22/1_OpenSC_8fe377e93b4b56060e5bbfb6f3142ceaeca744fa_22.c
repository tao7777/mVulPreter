static int enum_dir(sc_path_t path, int depth)
 {
 	sc_file_t *file;
 	int r, file_type;
	u8 files[SC_MAX_APDU_BUFFER_SIZE];
 
 	r = sc_lock(card);
 	if (r == SC_SUCCESS)
		r = sc_select_file(card, &path, &file);
	sc_unlock(card);
	if (r) {
		fprintf(stderr, "SELECT FILE failed: %s\n", sc_strerror(r));
		return 1;
	}
	print_file(card, file, &path, depth);
	file_type = file->type;
	sc_file_free(file);
	if (file_type == SC_FILE_TYPE_DF) {
		int i;

		r = sc_lock(card);
		if (r == SC_SUCCESS)
			r = sc_list_files(card, files, sizeof(files));
		sc_unlock(card);
		if (r < 0) {
			fprintf(stderr, "sc_list_files() failed: %s\n", sc_strerror(r));
			return 1;
 		}
 		if (r == 0) {
 			printf("Empty directory\n");
		} else
		for (i = 0; i < r/2; i++) {
			sc_path_t tmppath;
			memset(&tmppath, 0, sizeof(tmppath));
			memcpy(&tmppath, &path, sizeof(path));
			memcpy(tmppath.value + tmppath.len, files + 2*i, 2);
			tmppath.len += 2;
			enum_dir(tmppath, depth + 1);
 		}
 	}
 	return 0;
}
