 smb2_flush(smb_request_t *sr)
 {
 	uint16_t StructSize;
 	uint16_t reserved1;
 	uint32_t reserved2;
	smb2fid_t smb2fid;
	uint32_t status;
	int rc = 0;

	/*
	 * SMB2 Flush request
	 */
	rc = smb_mbc_decodef(
	    &sr->smb_data, "wwlqq",
	    &StructSize,		/* w */
	    &reserved1,			/* w */
	    &reserved2,			/* l */
	    &smb2fid.persistent,	/* q */
	    &smb2fid.temporal);		/* q */
	if (rc)
		return (SDRC_ERROR);
	if (StructSize != 24)
		return (SDRC_ERROR);

	status = smb2sr_lookup_fid(sr, &smb2fid);
	if (status) {
 		smb2sr_put_error(sr, status);
 		return (SDRC_SUCCESS);
 	}
 
	smb_ofile_flush(sr, sr->fid_ofile);
 
 	/*
 	 * SMB2 Flush reply
	 */
	(void) smb_mbc_encodef(
	    &sr->reply, "wwl",
	    4,	/* StructSize */	/* w */
	    0); /* reserved */		/* w */

	return (SDRC_SUCCESS);
}
