static int read_public_key(RSA *rsa)
{
	int r;
	sc_path_t path;
	sc_file_t *file;
	u8 buf[2048], *p = buf;
	size_t bufsize, keysize;

	r = select_app_df();
	if (r)
		return 1;
	sc_format_path("I1012", &path);
	r = sc_select_file(card, &path, &file);
	if (r) {
 		fprintf(stderr, "Unable to select public key file: %s\n", sc_strerror(r));
 		return 2;
 	}
	bufsize = file->size;
 	sc_file_free(file);
 	r = sc_read_binary(card, 0, buf, bufsize, 0);
 	if (r < 0) {
		fprintf(stderr, "Unable to read public key file: %s\n", sc_strerror(r));
		return 2;
	}
	bufsize = r;
	do {
		if (bufsize < 4)
			return 3;
		keysize = (p[0] << 8) | p[1];
		if (keysize == 0)
			break;
		if (keysize < 3)
			return 3;
		if (p[2] == opt_key_num)
			break;
		p += keysize;
		bufsize -= keysize;
	} while (1);
	if (keysize == 0) {
		printf("Key number %d not found.\n", opt_key_num);
		return 2;
	}
	return parse_public_key(p, keysize, rsa);
}
