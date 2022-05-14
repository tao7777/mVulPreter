static M_fs_error_t M_fs_copy_file(const char *path_old, const char *path_new, M_fs_file_mode_t mode, M_fs_progress_cb_t cb, M_fs_progress_flags_t progress_flags, M_fs_progress_t *progress, const M_fs_perms_t *perms)
{
	M_fs_file_t   *fd_old;
	M_fs_file_t   *fd_new;
	M_fs_info_t   *info         = NULL;
	unsigned char  temp[M_FS_BUF_SIZE];
	size_t         read_len;
	size_t         wrote_len;
	size_t         wrote_total  = 0;
 	size_t         offset;
 	M_fs_error_t   res;
 
 	/* Open the old file */
 	res = M_fs_file_open(&fd_old, path_old, M_FS_BUF_SIZE, M_FS_FILE_MODE_READ|M_FS_FILE_MODE_NOCREATE, NULL);
 	if (res != M_FS_ERROR_SUCCESS) {
		return res;
	}

	if (perms == NULL && mode & M_FS_FILE_MODE_PRESERVE_PERMS) {
		res = M_fs_info_file(&info, fd_old, M_FS_PATH_INFO_FLAGS_NONE);
		if (res != M_FS_ERROR_SUCCESS) {
			M_fs_file_close(fd_old);
			return res;
 		}
 		perms = M_fs_info_get_perms(info);
 	}

	/* We're going to create/open/truncate the new file, then as we read the contents from the old file we'll write it
	 * to new file. */
 	res = M_fs_file_open(&fd_new, path_new, M_FS_BUF_SIZE, M_FS_FILE_MODE_WRITE|M_FS_FILE_MODE_OVERWRITE, perms);
 	M_fs_info_destroy(info);
 	if (res != M_FS_ERROR_SUCCESS) {
		M_fs_file_close(fd_old);
		return res;
	}

	/* Copy the contents of old into new. */
	while ((res = M_fs_file_read(fd_old, temp, sizeof(temp), &read_len, M_FS_FILE_RW_NORMAL)) == M_FS_ERROR_SUCCESS && read_len != 0) {
		offset = 0;
		while (offset < read_len) {
			res          = M_fs_file_write(fd_new, temp+offset, read_len-offset, &wrote_len, M_FS_FILE_RW_NORMAL);
			offset      += wrote_len;
			wrote_total += wrote_len;

			if (cb) {
				M_fs_progress_set_result(progress, res);
				if (progress_flags & M_FS_PROGRESS_SIZE_TOTAL) {
					M_fs_progress_set_size_total_progess(progress, M_fs_progress_get_size_total_progess(progress)+wrote_len);
				}
				if (progress_flags & M_FS_PROGRESS_SIZE_CUR) {
					M_fs_progress_set_size_current_progress(progress, wrote_total);
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
		if (res != M_FS_ERROR_SUCCESS) {
			break;
		}
	}
	M_fs_file_close(fd_old);
	M_fs_file_close(fd_new);
	if (res != M_FS_ERROR_SUCCESS) {
		return res;
	}

	return M_FS_ERROR_SUCCESS;
}
