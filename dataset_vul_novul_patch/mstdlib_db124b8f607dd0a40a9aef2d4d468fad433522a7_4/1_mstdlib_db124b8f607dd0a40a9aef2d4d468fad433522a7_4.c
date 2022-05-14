M_bool M_fs_path_ishidden(const char *path, M_fs_info_t *info)
{
	M_list_str_t *path_parts;
	size_t        len;
	M_bool        ret        = M_FALSE;

	(void)info;

	if (path == NULL || *path == '\0') {
		return M_FALSE;
 	}
 
 	/* Hidden. Check if the first character of the last part of the path. Either the file or directory name itself
 	 * starts with a '.'. */
 	path_parts = M_fs_path_componentize_path(path, M_FS_SYSTEM_UNIX);
 	len = M_list_str_len(path_parts);
 	if (len > 0) {
		if (*M_list_str_at(path_parts, len-1) == '.') {
			ret = M_TRUE;
		}
	}
	M_list_str_destroy(path_parts);

	return ret;
}
