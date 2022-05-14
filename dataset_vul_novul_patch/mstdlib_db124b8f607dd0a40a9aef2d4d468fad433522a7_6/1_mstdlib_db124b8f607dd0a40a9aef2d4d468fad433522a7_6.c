char *M_fs_path_tmpdir(M_fs_system_t sys_type)
{
	char         *d   = NULL;
	char         *out = NULL;
	M_fs_error_t  res;

#ifdef _WIN32
	size_t len = M_fs_path_get_path_max(M_FS_SYSTEM_WINDOWS)+1;
	d = M_malloc_zero(len);
	/* Return is length without NULL. */
	if (GetTempPath((DWORD)len, d) >= len) {
		M_free(d);
		d = NULL;
	}
#elif defined(__APPLE__)
 	d = M_fs_path_mac_tmpdir();
 #else
 	const char *const_temp;
	/* Try Unix env var. */
 #  ifdef HAVE_SECURE_GETENV
 	const_temp = secure_getenv("TMPDIR");
 #  else
	const_temp = getenv("TMPDIR");
#  endif
	if (!M_str_isempty(const_temp) && M_fs_perms_can_access(const_temp, M_FS_FILE_MODE_READ|M_FS_FILE_MODE_WRITE) == M_FS_ERROR_SUCCESS) {
		d = M_strdup(const_temp);
	}
	/* Fallback to some "standard" system paths. */
	if (d == NULL) {
		const_temp = "/tmp";
		if (!M_str_isempty(const_temp) && M_fs_perms_can_access(const_temp, M_FS_FILE_MODE_READ|M_FS_FILE_MODE_WRITE) == M_FS_ERROR_SUCCESS) {
			d = M_strdup(const_temp);
		}
	}
	if (d == NULL) {
		const_temp = "/var/tmp";
		if (!M_str_isempty(const_temp) && M_fs_perms_can_access(const_temp, M_FS_FILE_MODE_READ|M_FS_FILE_MODE_WRITE) == M_FS_ERROR_SUCCESS) {
			d = M_strdup(const_temp);
		}
	}
#endif

	if (d != NULL) {
		res = M_fs_path_norm(&out, d, M_FS_PATH_NORM_ABSOLUTE, sys_type);
		if (res != M_FS_ERROR_SUCCESS) {
			out = NULL;
		}
	}
	M_free(d);

	return out;
}
