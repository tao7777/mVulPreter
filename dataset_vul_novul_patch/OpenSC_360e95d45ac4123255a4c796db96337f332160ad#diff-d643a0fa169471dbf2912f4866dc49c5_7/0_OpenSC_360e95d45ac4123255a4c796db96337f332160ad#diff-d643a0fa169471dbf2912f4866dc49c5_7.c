int sc_file_set_sec_attr(sc_file_t *file, const u8 *sec_attr,
			 size_t sec_attr_len)
{
	u8 *tmp;
	if (!sc_file_valid(file)) {
 		return SC_ERROR_INVALID_ARGUMENTS;
 	}
 
	if (sec_attr == NULL || sec_attr_len) {
 		if (file->sec_attr != NULL)
 			free(file->sec_attr);
 		file->sec_attr = NULL;
		file->sec_attr_len = 0;
		return 0;
	 }
	tmp = (u8 *) realloc(file->sec_attr, sec_attr_len);
	if (!tmp) {
		if (file->sec_attr)
			free(file->sec_attr);
		file->sec_attr     = NULL;
		file->sec_attr_len = 0;
		return SC_ERROR_OUT_OF_MEMORY;
	}
	file->sec_attr = tmp;
	memcpy(file->sec_attr, sec_attr, sec_attr_len);
	file->sec_attr_len = sec_attr_len;

	return 0;
}
