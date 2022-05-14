M_fs_error_t M_fs_move(const char *path_old, const char *path_new, M_uint32 mode, M_fs_progress_cb_t cb, M_uint32 progress_flags)
{
	char            *norm_path_old;
	char            *norm_path_new;
	char            *resolve_path;
	M_fs_info_t     *info;
	M_fs_progress_t *progress      = NULL;
	M_uint64         entry_size;
	M_fs_error_t     res;

	if (path_old == NULL || *path_old == '\0' || path_new == NULL || *path_new == '\0') {
		return M_FS_ERROR_INVALID;
	}

	/* It's okay if new path doesn't exist. */
	res = M_fs_path_norm(&norm_path_new, path_new, M_FS_PATH_NORM_RESDIR, M_FS_SYSTEM_AUTO);
	if (res != M_FS_ERROR_SUCCESS) {
		M_free(norm_path_new);
		return res;
	}

	/* If a path is a file and the destination is a directory the file should be moved 
	 * into the directory. E.g. /file.txt -> /dir = /dir/file.txt */
	if (M_fs_isfileintodir(path_old, path_new, &norm_path_old)) {
		M_free(norm_path_new);
		res = M_fs_move(path_old, norm_path_old, mode, cb, progress_flags);
		M_free(norm_path_old);
		return res;
 	}
 
 	/* Normalize the old path and do basic checks that it exists. We'll leave really checking that the old path
 	 * existing to rename because any check we perform may not be true when rename is called. */
 	res = M_fs_path_norm(&norm_path_old, path_old, M_FS_PATH_NORM_RESALL, M_FS_SYSTEM_AUTO);
 	if (res != M_FS_ERROR_SUCCESS) {
 		M_free(norm_path_new);
		M_free(norm_path_old);
		return res;
	}

	progress = M_fs_progress_create();

	res = M_fs_info(&info, path_old, (mode & M_FS_FILE_MODE_PRESERVE_PERMS)?M_FS_PATH_INFO_FLAGS_NONE:M_FS_PATH_INFO_FLAGS_BASIC);
	if (res != M_FS_ERROR_SUCCESS) {
		M_fs_progress_destroy(progress);
		M_free(norm_path_new);
		M_free(norm_path_old);
 		return res;
 	}
 
 	/* There is a race condition where the path could not exist but be created between the exists check and calling
 	 * rename to move the file but there isn't much we can do in this case. copy will delete and the file so this
 	 * situation won't cause an error. */
 	if (!M_fs_check_overwrite_allowed(norm_path_old, norm_path_new, mode)) {
		M_fs_progress_destroy(progress);
		M_free(norm_path_new);
		M_free(norm_path_old);
		return M_FS_ERROR_FILE_EXISTS;
	}

	if (cb) {
		entry_size = M_fs_info_get_size(info);

		M_fs_progress_set_path(progress, norm_path_new);
		M_fs_progress_set_type(progress, M_fs_info_get_type(info));
		if (progress_flags & M_FS_PROGRESS_SIZE_TOTAL) {
			M_fs_progress_set_size_total(progress, entry_size);
			M_fs_progress_set_size_total_progess(progress, entry_size);
		}
		if (progress_flags & M_FS_PROGRESS_SIZE_CUR) {
			M_fs_progress_set_size_current(progress, entry_size);
			M_fs_progress_set_size_current_progress(progress, entry_size);
		}
		/* Change the progress count to reflect the count. */
		if (progress_flags & M_FS_PROGRESS_COUNT) {
			M_fs_progress_set_count_total(progress, 1);
			M_fs_progress_set_count(progress, 1);
		}
	}

	/* Move the file. */
	if (M_fs_info_get_type(info) == M_FS_TYPE_SYMLINK) {
		res = M_fs_path_readlink(&resolve_path, norm_path_old);
		if (res == M_FS_ERROR_SUCCESS) {
			res = M_fs_symlink(norm_path_new, resolve_path);
		}
		M_free(resolve_path);
	} else {
		res = M_fs_move_file(norm_path_old, norm_path_new);
	}
	/* Failure was because we're crossing mount points. */
	if (res == M_FS_ERROR_NOT_SAMEDEV) {
		/* Can't rename so copy and delete. */
		if (M_fs_copy(norm_path_old, norm_path_new, mode, cb, progress_flags) == M_FS_ERROR_SUCCESS) {
			/* Success - Delete the original files since this is a move. */
 			res = M_fs_delete(norm_path_old, M_TRUE, NULL, M_FS_PROGRESS_NOEXTRA);
 		} else {
 			/* Failure - Delete the new files that were copied but only if we are not overwriting. We don't
 			 * want to remove any existing files (especially if the dest is a dir). */
 			if (!(mode & M_FS_FILE_MODE_OVERWRITE)) {
 				M_fs_delete(norm_path_new, M_TRUE, NULL, M_FS_PROGRESS_NOEXTRA);
 			}
 			res = M_FS_ERROR_GENERIC;
 		}
 	} else {
 		/* Call the cb with the result of the move whether it was a success for fail. We call the cb only if the
 		 * result of the move is not M_FS_ERROR_NOT_SAMEDEV because the copy operation will call the cb for us. */
 		if (cb) {
 			M_fs_progress_set_result(progress, res);
 			if (!cb(progress)) {
				res = M_FS_ERROR_CANCELED;
			}
		}
	}

	M_fs_info_destroy(info);
	M_fs_progress_destroy(progress);
	M_free(norm_path_new);
	M_free(norm_path_old);

	return res;
}
