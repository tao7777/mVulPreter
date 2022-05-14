static NTSTATUS fd_open_atomic(struct connection_struct *conn,
			files_struct *fsp,
			int flags,
			mode_t mode,
                        bool *file_created)
 {
        NTSTATUS status = NT_STATUS_UNSUCCESSFUL;
        bool file_existed = VALID_STAT(fsp->fsp_name->st);
 
        *file_created = false;
 
		 * We're not creating the file, just pass through.
		 */
		return fd_open(conn, fsp, flags, mode);
	}
