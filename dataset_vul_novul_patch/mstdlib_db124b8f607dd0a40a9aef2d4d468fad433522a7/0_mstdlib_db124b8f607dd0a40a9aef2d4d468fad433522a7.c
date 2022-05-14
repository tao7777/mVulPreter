 static M_bool M_fs_check_overwrite_allowed(const char *p1, const char *p2, M_uint32 mode)
 {
 	M_fs_info_t  *info = NULL;
	char         *pold = NULL;
	char         *pnew = NULL;
	M_fs_type_t   type;
	M_bool        ret  = M_TRUE;

	if (mode & M_FS_FILE_MODE_OVERWRITE)
		return M_TRUE;

	/* If we're not overwriting we need to verify existance.
 	 *
 	 * For files we need to check if the file name exists in the
	 * directory it's being copied to.
	 *
	 * For directories we need to check if the directory name
	 * exists in the directory it's being copied to.
	 */

	if (M_fs_info(&info, p1, M_FS_PATH_INFO_FLAGS_BASIC) != M_FS_ERROR_SUCCESS)
		return M_FALSE;

	type = M_fs_info_get_type(info);
	M_fs_info_destroy(info);
 
 	if (type != M_FS_TYPE_DIR) {
 		/* File exists at path. */
		if (M_fs_perms_can_access(p2, M_FS_PERMS_MODE_NONE) == M_FS_ERROR_SUCCESS) {
 			ret = M_FALSE;
 			goto done;
 		}
	}

	/* Is dir */
	pold = M_fs_path_basename(p1, M_FS_SYSTEM_AUTO);
	pnew = M_fs_path_join(p2, pnew, M_FS_SYSTEM_AUTO);
	if (M_fs_perms_can_access(pnew, M_FS_PERMS_MODE_NONE) == M_FS_ERROR_SUCCESS) {
		ret = M_FALSE;
		goto done;
	}

done:
	M_free(pnew);
	M_free(pold);
	return ret;
}
