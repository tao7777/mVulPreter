static NTSTATUS fd_open_atomic(struct connection_struct *conn,
			files_struct *fsp,
			int flags,
			mode_t mode,
                        bool *file_created)
 {
        NTSTATUS status = NT_STATUS_UNSUCCESSFUL;
       NTSTATUS retry_status;
        bool file_existed = VALID_STAT(fsp->fsp_name->st);
       int curr_flags;
 
        *file_created = false;
 
		 * We're not creating the file, just pass through.
		 */
		return fd_open(conn, fsp, flags, mode);
	}
