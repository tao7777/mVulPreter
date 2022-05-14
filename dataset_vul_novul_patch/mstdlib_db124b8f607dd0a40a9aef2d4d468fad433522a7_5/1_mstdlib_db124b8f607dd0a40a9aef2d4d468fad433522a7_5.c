char *M_fs_path_join_parts(const M_list_str_t *path, M_fs_system_t sys_type)
{
	M_list_str_t *parts;
	const char   *part;
	char         *out;
	size_t        len;
	size_t        i;
	size_t        count;

	if (path == NULL) {
		return NULL;
	}
	len = M_list_str_len(path);
	if (len == 0) {
		return NULL;
	}

 	sys_type = M_fs_path_get_system_type(sys_type);
 
 	/* Remove any empty parts (except for the first part which denotes an abs path on Unix
 	 * or a UNC path on Windows). */
 	parts = M_list_str_duplicate(path);
 	for (i=len-1; i>0; i--) {
 		part = M_list_str_at(parts, i);
		if (part == NULL || *part == '\0') {
			M_list_str_remove_at(parts, i);
		}
	}

	len = M_list_str_len(parts);

	/* Join puts the sep between items. If there are no items then the sep
	 * won't be written. */
	part = M_list_str_at(parts, 0);
	if (len == 1 && (part == NULL || *part == '\0')) {
		M_list_str_destroy(parts);
		if (sys_type == M_FS_SYSTEM_WINDOWS) {
			return M_strdup("\\\\");
		}
		return M_strdup("/");
	}

	/* Handle windows abs path because they need two separators. */
	if (sys_type == M_FS_SYSTEM_WINDOWS && len > 0) {
		part  = M_list_str_at(parts, 0);
		/* If we have 1 item we need to add two empties so we get the second separator. */
		count = (len == 1) ? 2 : 1;
		/* If we're dealing with a unc path add the second sep so we get two separators for the UNC base. */
		if (part != NULL && *part == '\0') {
			for (i=0; i<count; i++) {
				M_list_str_insert_at(parts, "", 0);
			}
		} else if (M_fs_path_isabs(part, sys_type) && len == 1) {
			/* We need to add an empty so we get a separator after the drive. */
			M_list_str_insert_at(parts, "", 1);
		}
	}

	out = M_list_str_join(parts, (unsigned char)M_fs_path_get_system_sep(sys_type));
	M_list_str_destroy(parts);
	return out;
}
