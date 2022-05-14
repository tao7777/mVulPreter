spnego_gss_import_sec_context(
	OM_uint32		*minor_status,
 	const gss_buffer_t	interprocess_token,
 	gss_ctx_id_t		*context_handle)
 {
	OM_uint32 ret;
	ret = gss_import_sec_context(minor_status,
				    interprocess_token,
				    context_handle);
	return (ret);
 }
