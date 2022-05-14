M_fs_error_t M_fs_copy(const char *path_old, const char *path_new, M_uint32 mode, M_fs_progress_cb_t cb, M_uint32 progress_flags)
{
	char                   *norm_path_old;
	char                   *norm_path_new;
	char                   *join_path_old;
	char                   *join_path_new;
	M_fs_dir_entries_t     *entries;
	const M_fs_dir_entry_t *entry;
	M_fs_info_t            *info;
	M_fs_progress_t        *progress            = NULL;
	M_fs_dir_walk_filter_t  filter              = M_FS_DIR_WALK_FILTER_ALL|M_FS_DIR_WALK_FILTER_RECURSE;
	M_fs_type_t             type;
	size_t                  len;
	size_t                  i;
	M_uint64                total_count         = 0;
	M_uint64                total_size          = 0;
	M_uint64                total_size_progress = 0;
	M_uint64                entry_size;
	M_fs_error_t            res;

	if (path_old == NULL || *path_old == '\0' || path_new == NULL || *path_new == '\0') {
		return M_FS_ERROR_INVALID;
	}

	/* It's okay if new path doesn't exist. */
	res = M_fs_path_norm(&norm_path_new, path_new, M_FS_PATH_NORM_RESDIR, M_FS_SYSTEM_AUTO);
	if (res != M_FS_ERROR_SUCCESS) {
		M_free(norm_path_new);
		return res;
	}

	/* If a path is a file and the destination is a directory the file should be copied
	 * into the directory. E.g. /file.txt -> /dir = /dir/file.txt */
	if (M_fs_isfileintodir(path_old, path_new, &norm_path_old)) {
		M_free(norm_path_new);
		res = M_fs_copy(path_old, norm_path_old, mode, cb, progress_flags);
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
 
 	type = M_fs_info_get_type(info);
 
 	/* There is a race condition where the path could not exist but be created between the exists check and calling
 	 * rename to move the file but there isn't much we can do in this case. copy will delete and the file so this
 	 * situation won't cause an error. */
 	if (!M_fs_check_overwrite_allowed(norm_path_old, norm_path_new, mode)) {
		M_fs_progress_destroy(progress);
		M_free(norm_path_new);
		M_free(norm_path_old);
		return M_FS_ERROR_FILE_EXISTS;
	}
 
 	entries = M_fs_dir_entries_create();
 	/* No need to destroy info  because it's now owned by entries and will be destroyed when entries is destroyed.
 	 * M_FS_DIR_WALK_FILTER_READ_INFO_BASIC doesn't actually get the perms it's just there to ensure the info is
 	 * stored in the entry. */
 	M_fs_dir_entries_insert(entries, M_fs_dir_walk_fill_entry(norm_path_new, NULL, type, info, M_FS_DIR_WALK_FILTER_READ_INFO_BASIC));
 	if (type == M_FS_TYPE_DIR) {
		if (mode & M_FS_FILE_MODE_PRESERVE_PERMS) {
			filter |= M_FS_DIR_WALK_FILTER_READ_INFO_FULL;
		} else if (cb && progress_flags & (M_FS_PROGRESS_SIZE_TOTAL|M_FS_PROGRESS_SIZE_CUR)) {
			filter |= M_FS_DIR_WALK_FILTER_READ_INFO_BASIC;
		}
		/* Get all the files under the dir. */
		M_fs_dir_entries_merge(&entries, M_fs_dir_walk_entries(norm_path_old, NULL, filter));
	}

	/* Put all dirs first. We need to ensure the dir(s) exist before we can copy files. */
	M_fs_dir_entries_sort(entries, M_FS_DIR_SORT_ISDIR, M_TRUE, M_FS_DIR_SORT_NAME_CASECMP, M_TRUE);

	len = M_fs_dir_entries_len(entries);
	if (cb) {
		total_size = 0;
		for (i=0; i<len; i++) {
			entry       = M_fs_dir_entries_at(entries, i);
			entry_size  = M_fs_info_get_size(M_fs_dir_entry_get_info(entry));
			total_size += entry_size;
 
 			type = M_fs_dir_entry_get_type(entry);
 			/* The total isn't the total number of files but the total number of operations. 
 			 * Making dirs and symlinks is one operation and copying a file will be split into
 			 * multiple operations. Copying uses the M_FS_BUF_SIZE to read and write in
 			 * chunks. We determine how many chunks will be needed to read the entire file and
 			 * use that for the number of operations for the file. */
			if (type == M_FS_TYPE_DIR || type == M_FS_TYPE_SYMLINK) {
				total_count++;
			} else {
				total_count += (entry_size + M_FS_BUF_SIZE - 1) / M_FS_BUF_SIZE;
			}
		}
		/* Change the progress total size to reflect all entries. */
		if (progress_flags & M_FS_PROGRESS_SIZE_TOTAL) {
			M_fs_progress_set_size_total(progress, total_size);
		}
		/* Change the progress count to reflect the count. */
		if (progress_flags & M_FS_PROGRESS_COUNT) {
			M_fs_progress_set_count_total(progress, total_count);
		}
	}
	for (i=0; i<len; i++) {
		entry         = M_fs_dir_entries_at(entries, i);
		type          = M_fs_dir_entry_get_type(entry);
		join_path_old = M_fs_path_join(norm_path_old, M_fs_dir_entry_get_name(entry), M_FS_SYSTEM_AUTO);
		join_path_new = M_fs_path_join(norm_path_new, M_fs_dir_entry_get_name(entry), M_FS_SYSTEM_AUTO);

		entry_size           = M_fs_info_get_size(M_fs_dir_entry_get_info(entry));
		total_size_progress += entry_size;

		if (cb) {
			M_fs_progress_set_path(progress, join_path_new);
			if (progress_flags & M_FS_PROGRESS_SIZE_CUR) {
				M_fs_progress_set_size_current(progress, entry_size);
			}
		}

		/* op */
		if (type == M_FS_TYPE_DIR || type == M_FS_TYPE_SYMLINK) {
			if (type == M_FS_TYPE_DIR) {
				res = M_fs_dir_mkdir(join_path_new, M_FALSE, NULL);
			} else if (type == M_FS_TYPE_SYMLINK) {
				res = M_fs_symlink(join_path_new, M_fs_dir_entry_get_resolved_name(entry));
			} 
			if (res == M_FS_ERROR_SUCCESS && (mode & M_FS_FILE_MODE_PRESERVE_PERMS)) {
				res = M_fs_perms_set_perms(M_fs_info_get_perms(M_fs_dir_entry_get_info(entry)), join_path_new);
			}
		} else {
			res = M_fs_copy_file(join_path_old, join_path_new, mode, cb, progress_flags, progress, M_fs_info_get_perms(M_fs_dir_entry_get_info(entry)));
		}

		M_free(join_path_old);
		M_free(join_path_new);

		/* Call the callback and stop processing if requested. */
		if ((type == M_FS_TYPE_DIR || type == M_FS_TYPE_SYMLINK) && cb) {
			M_fs_progress_set_type(progress, M_fs_dir_entry_get_type(entry));
			M_fs_progress_set_result(progress, res);

			if (progress_flags & M_FS_PROGRESS_SIZE_TOTAL) {
				M_fs_progress_set_size_total_progess(progress, total_size_progress);
			}
			if (progress_flags & M_FS_PROGRESS_SIZE_CUR) {
				M_fs_progress_set_size_current_progress(progress, entry_size);
			}
			if (progress_flags & M_FS_PROGRESS_COUNT) {
				M_fs_progress_set_count(progress, M_fs_progress_get_count(progress)+1);
			}

			if (!cb(progress)) {
				res = M_FS_ERROR_CANCELED;
			}
		}
		if (res != M_FS_ERROR_SUCCESS) {
			break;
		}
 	}
 
 	/* Delete the file(s) if it could not be copied properly, but only if we are not overwriting.
 	 * If we're overwriting then there could be other files in that location (especially if it's a dir). */
 	if (res != M_FS_ERROR_SUCCESS && !(mode & M_FS_FILE_MODE_OVERWRITE)) {
 		M_fs_delete(path_new, M_TRUE, NULL, M_FS_PROGRESS_NOEXTRA);
 	}

	M_fs_dir_entries_destroy(entries);
	M_fs_progress_destroy(progress);
	M_free(norm_path_new);
	M_free(norm_path_old);

	return res;
}
