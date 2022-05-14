smb_flush_file(struct smb_request *sr, struct smb_ofile *ofile)
{
	sr->user_cr = smb_ofile_getcred(ofile);
	if ((ofile->f_node->flags & NODE_FLAGS_WRITE_THROUGH) == 0)
		(void) smb_fsop_commit(sr, sr->user_cr, ofile->f_node);
}
