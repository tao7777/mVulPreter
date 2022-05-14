smb_com_flush(smb_request_t *sr)
{
	smb_ofile_t	*file;
	smb_llist_t	*flist;
	int		rc;

	if (smb_flush_required == 0) {
		rc = smbsr_encode_empty_result(sr);
		return ((rc == 0) ? SDRC_SUCCESS : SDRC_ERROR);
	}

	if (sr->smb_fid != 0xffff) {
		smbsr_lookup_file(sr);
		if (sr->fid_ofile == NULL) {
			smbsr_error(sr, NT_STATUS_INVALID_HANDLE,
 			    ERRDOS, ERRbadfid);
 			return (SDRC_ERROR);
 		}
		smb_flush_file(sr, sr->fid_ofile);
 	} else {
 		flist = &sr->tid_tree->t_ofile_list;
 		smb_llist_enter(flist, RW_READER);
 		file = smb_llist_head(flist);
 		while (file) {
 			mutex_enter(&file->f_mutex);
			smb_flush_file(sr, file);
 			mutex_exit(&file->f_mutex);
 			file = smb_llist_next(flist, file);
 		}
		smb_llist_exit(flist);
	}

 	rc = smbsr_encode_empty_result(sr);
 	return ((rc == 0) ? SDRC_SUCCESS : SDRC_ERROR);
 }
