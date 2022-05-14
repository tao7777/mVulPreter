spnego_gss_complete_auth_token(
		OM_uint32 *minor_status,
		const gss_ctx_id_t context_handle,
 		gss_buffer_t input_message_buffer)
 {
 	OM_uint32 ret;
	spnego_gss_ctx_id_t sc = (spnego_gss_ctx_id_t)context_handle;

	if (sc->ctx_handle == GSS_C_NO_CONTEXT)
		return (GSS_S_UNAVAILABLE);

 	ret = gss_complete_auth_token(minor_status,
				      sc->ctx_handle,
 				      input_message_buffer);
 	return (ret);
 }
