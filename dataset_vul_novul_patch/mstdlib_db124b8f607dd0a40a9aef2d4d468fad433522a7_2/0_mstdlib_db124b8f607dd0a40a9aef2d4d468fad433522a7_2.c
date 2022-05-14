M_fs_error_t M_fs_delete(const char *path, M_bool remove_children, M_fs_progress_cb_t cb, M_uint32 progress_flags)
{
	char                   *norm_path;
	char                   *join_path;
	M_fs_dir_entries_t     *entries;
	const M_fs_dir_entry_t *entry;
	M_fs_info_t            *info;
	M_fs_progress_t        *progress            = NULL;
	M_fs_dir_walk_filter_t  filter              = M_FS_DIR_WALK_FILTER_ALL|M_FS_DIR_WALK_FILTER_RECURSE;
	M_fs_type_t             type;
	/* The result that will be returned by this function. */
	M_fs_error_t            res;
	/* The result of the delete itself. */
	M_fs_error_t            res2;
	size_t                  len;
	size_t                  i;
	M_uint64                total_size          = 0;
	M_uint64                total_size_progress = 0;
	M_uint64                entry_size;

	/* Normalize the path we are going to delete so we have a valid path to pass around. */
	res = M_fs_path_norm(&norm_path, path, M_FS_PATH_NORM_HOME, M_FS_SYSTEM_AUTO);
	if (res != M_FS_ERROR_SUCCESS) {
		M_free(norm_path);
		return res;
	}

	/* We need the info to determine if the path is valid and because we need the type. */
	res = M_fs_info(&info, norm_path, M_FS_PATH_INFO_FLAGS_BASIC);
	if (res != M_FS_ERROR_SUCCESS) {
		M_free(norm_path);
		return res;
	}

	/* We must know the type because there are different functions for deleting a file and deleting a directory. */
	type = M_fs_info_get_type(info);
	if (type == M_FS_TYPE_UNKNOWN) {
		M_fs_info_destroy(info);
		M_free(norm_path);
		return M_FS_ERROR_GENERIC;
	}

	/* Create a list of entries to store all the places we need to delete. */
 	entries = M_fs_dir_entries_create();
 
 	/* Recursive directory deletion isn't intuitive. We have to generate a list of files and delete the list.
	 * We cannot delete as walk because not all file systems support that operation. The walk; delete; behavior
 	 * is undefined in Posix and HFS is known to skip files if the directory contents is modifies as the
 	 * directory is being walked. */
 	if (type == M_FS_TYPE_DIR && remove_children) {
		/* We need to read the basic info if the we need to report the size totals to the cb. */
		if (cb && progress_flags & (M_FS_PROGRESS_SIZE_TOTAL|M_FS_PROGRESS_SIZE_CUR)) {
			filter |= M_FS_DIR_WALK_FILTER_READ_INFO_BASIC;
		}
		M_fs_dir_entries_merge(&entries, M_fs_dir_walk_entries(norm_path, NULL, filter));
 	}
 
 	/* Add the original path to the list of entries. This may be the only entry in the list. We need to add
	 * it after a potential walk because we can't delete a directory that isn't empty.
 	 * Note: 
 	 *   - The info will be owned by the entry and destroyed when it is destroyed. 
 	 *   - The basic info param doesn't get the info in this case. it's set so the info is stored in the entry. */
	M_fs_dir_entries_insert(entries, M_fs_dir_walk_fill_entry(norm_path, NULL, type, info, M_FS_DIR_WALK_FILTER_READ_INFO_BASIC));

 	len = M_fs_dir_entries_len(entries);
 	if (cb) {
 		/* Create the progress. The same progress will be used for the entire operation. It will be updated with
		 * new info as necessary. */
 		progress = M_fs_progress_create();
 
 		/* Get the total size of all files to be deleted if using the progress cb and size totals is set. */
		if (progress_flags & M_FS_PROGRESS_SIZE_TOTAL) {
			for (i=0; i<len; i++) {
				entry       = M_fs_dir_entries_at(entries, i);
				entry_size  = M_fs_info_get_size(M_fs_dir_entry_get_info(entry));
				total_size += entry_size;
			}
			/* Change the progress total size to reflect all entries. */
			M_fs_progress_set_size_total(progress, total_size);
		}
		/* Change the progress count to reflect the count. */
		if (progress_flags & M_FS_PROGRESS_COUNT) {
			M_fs_progress_set_count_total(progress, len);
		}
	}

	/* Assume success. Set error if there is an error. */
	res = M_FS_ERROR_SUCCESS;
	/* Loop though all entries and delete. */
	for (i=0; i<len; i++) {
		entry     = M_fs_dir_entries_at(entries, i);
		join_path = M_fs_path_join(norm_path, M_fs_dir_entry_get_name(entry), M_FS_SYSTEM_AUTO);
		/* Call the appropriate delete function. */
		if (M_fs_dir_entry_get_type(entry) == M_FS_TYPE_DIR) {
			res2 = M_fs_delete_dir(join_path);
		} else {
			res2 = M_fs_delete_file(join_path);
		}
		/* Set the return result to denote there was an error. The real error will be sent via the
		 * progress callback for the entry. */
		if (res2 != M_FS_ERROR_SUCCESS) {
			res = M_FS_ERROR_GENERIC;
		}
		/* Set the progress data for the entry. */
		if (cb) {
			entry_size           = M_fs_info_get_size(M_fs_dir_entry_get_info(entry));
			total_size_progress += entry_size;

			M_fs_progress_set_path(progress, join_path);
			M_fs_progress_set_type(progress, M_fs_dir_entry_get_type(entry));
			M_fs_progress_set_result(progress, res2);
			if (progress_flags & M_FS_PROGRESS_COUNT) {
				M_fs_progress_set_count(progress, i+1);
			}
			if (progress_flags & M_FS_PROGRESS_SIZE_TOTAL) {
				M_fs_progress_set_size_total_progess(progress, total_size_progress);
			}
			if (progress_flags & M_FS_PROGRESS_SIZE_CUR) {
				M_fs_progress_set_size_current(progress, entry_size);
				M_fs_progress_set_size_current_progress(progress, entry_size);
			}
		}
		M_free(join_path);
		/* Call the callback and stop processing if requested. */
		if (cb && !cb(progress)) {
			res = M_FS_ERROR_CANCELED;
			break;
		}
	}

	M_fs_dir_entries_destroy(entries);
	M_fs_progress_destroy(progress);
	M_free(norm_path);
	return res;
}
