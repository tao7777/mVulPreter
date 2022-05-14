PHYSICALPATH_FUNC(mod_alias_physical_handler) {
	plugin_data *p = p_d;
	int uri_len, basedir_len;
	char *uri_ptr;
	size_t k;

	if (buffer_is_empty(con->physical.path)) return HANDLER_GO_ON;

	mod_alias_patch_connection(srv, con, p);

	/* not to include the tailing slash */
	basedir_len = buffer_string_length(con->physical.basedir);
	if ('/' == con->physical.basedir->ptr[basedir_len-1]) --basedir_len;
	uri_len = buffer_string_length(con->physical.path) - basedir_len;
	uri_ptr = con->physical.path->ptr + basedir_len;

	for (k = 0; k < p->conf.alias->used; k++) {
		data_string *ds = (data_string *)p->conf.alias->data[k];
		int alias_len = buffer_string_length(ds->key);

		if (alias_len > uri_len) continue;
		if (buffer_is_empty(ds->key)) continue;

		if (0 == (con->conf.force_lowercase_filenames ?
					strncasecmp(uri_ptr, ds->key->ptr, alias_len) :
 					strncmp(uri_ptr, ds->key->ptr, alias_len))) {
 			/* matched */
 
			/* check for path traversal in url-path following alias if key
			 * does not end in slash, but replacement value ends in slash */
			if (uri_ptr[alias_len] == '.') {
				char *s = uri_ptr + alias_len + 1;
				if (*s == '.') ++s;
				if (*s == '/' || *s == '\0') {
					size_t vlen = buffer_string_length(ds->value);
					if (0 != alias_len && ds->key->ptr[alias_len-1] != '/'
					    && 0 != vlen && ds->value->ptr[vlen-1] == '/') {
						con->http_status = 403;
						return HANDLER_FINISHED;
					}
				}
			}

 			buffer_copy_buffer(con->physical.basedir, ds->value);
 			buffer_copy_buffer(srv->tmp_buf, ds->value);
 			buffer_append_string(srv->tmp_buf, uri_ptr + alias_len);
			buffer_copy_buffer(con->physical.path, srv->tmp_buf);

			return HANDLER_GO_ON;
		}
	}

	/* not found */
	return HANDLER_GO_ON;
}
