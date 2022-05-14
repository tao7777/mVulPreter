spnego_gss_process_context_token(
				OM_uint32	*minor_status,
				const gss_ctx_id_t context_handle,
 				const gss_buffer_t token_buffer)
 {
 	OM_uint32 ret;
	spnego_gss_ctx_id_t sc = (spnego_gss_ctx_id_t)context_handle;

	/* SPNEGO doesn't have its own context tokens. */
	if (!sc->opened)
		return (GSS_S_DEFECTIVE_TOKEN);

 	ret = gss_process_context_token(minor_status,
					sc->ctx_handle,
 					token_buffer);
 
 	return (ret);
}
