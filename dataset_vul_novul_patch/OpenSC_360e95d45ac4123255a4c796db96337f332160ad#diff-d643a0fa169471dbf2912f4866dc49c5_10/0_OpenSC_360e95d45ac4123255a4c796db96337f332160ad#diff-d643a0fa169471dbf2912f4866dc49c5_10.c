int read_file(struct sc_card *card, char *str_path, unsigned char **data, size_t *data_len)
{
	struct sc_path path;
	struct sc_file *file;
	unsigned char *p;
	int ok = 0;
	int r;
	size_t len;

	sc_format_path(str_path, &path);
	if (SC_SUCCESS != sc_select_file(card, &path, &file)) {
 		goto err;
 	}
 
	len = file && file->size > 0 ? file->size : 4096;
 	p = realloc(*data, len);
 	if (!p) {
 		goto err;
	}
	*data = p;
	*data_len = len;

	r = sc_read_binary(card, 0, p, len, 0);
	if (r < 0)
		goto err;

	*data_len = r;
	ok = 1;

err:
	sc_file_free(file);

	return ok;
}
