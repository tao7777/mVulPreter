spnego_gss_import_sec_context(
	OM_uint32		*minor_status,
 	const gss_buffer_t	interprocess_token,
 	gss_ctx_id_t		*context_handle)
 {
	/*
	 * Until we implement partial context exports, there are no SPNEGO
	 * exported context tokens, only tokens for underlying mechs.  So just
	 * return an error for now.
	 */
	return GSS_S_UNAVAILABLE;
 }
