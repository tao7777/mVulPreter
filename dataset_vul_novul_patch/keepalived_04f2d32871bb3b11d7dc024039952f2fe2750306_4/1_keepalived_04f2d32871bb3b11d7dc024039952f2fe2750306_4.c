vrrp_tfile_end_handler(void)
{
	vrrp_tracked_file_t *tfile = LIST_TAIL_DATA(vrrp_data->vrrp_track_files);
	struct stat statb;
	FILE *tf;
	int ret;

	if (!tfile->file_path) {
		report_config_error(CONFIG_GENERAL_ERROR, "No file set for track_file %s - removing", tfile->fname);
		free_list_element(vrrp_data->vrrp_track_files, vrrp_data->vrrp_track_files->tail);
		return;
	}

	if (track_file_init == TRACK_FILE_NO_INIT)
		return;

	ret = stat(tfile->file_path, &statb);
	if (!ret) {
		if (track_file_init == TRACK_FILE_CREATE) {
			/* The file exists */
			return;
		}
		if ((statb.st_mode & S_IFMT) != S_IFREG) {
			/* It is not a regular file */
			report_config_error(CONFIG_GENERAL_ERROR, "Cannot initialise track file %s - it is not a regular file", tfile->fname);
			return;
		}

		/* Don't overwrite a file on reload */
		if (reload)
			return;
	}
 
 	if (!__test_bit(CONFIG_TEST_BIT, &debug)) {
 		/* Write the value to the file */
		if ((tf = fopen(tfile->file_path, "w"))) {
 			fprintf(tf, "%d\n", track_file_init_value);
 			fclose(tf);
 		}
		else
			report_config_error(CONFIG_GENERAL_ERROR, "Unable to initialise track file %s", tfile->fname);
	}
}
