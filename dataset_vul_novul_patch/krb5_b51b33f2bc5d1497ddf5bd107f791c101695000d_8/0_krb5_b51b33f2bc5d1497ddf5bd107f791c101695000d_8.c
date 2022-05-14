spnego_gss_get_mic(
		OM_uint32 *minor_status,
		const gss_ctx_id_t context_handle,
		gss_qop_t  qop_req,
		const gss_buffer_t message_buffer,
 		gss_buffer_t message_token)
 {
 	OM_uint32 ret;
	spnego_gss_ctx_id_t sc = (spnego_gss_ctx_id_t)context_handle;

	if (sc->ctx_handle == GSS_C_NO_CONTEXT)
		return (GSS_S_NO_CONTEXT);

 	ret = gss_get_mic(minor_status,
		    sc->ctx_handle,
 		    qop_req,
 		    message_buffer,
 		    message_token);
	return (ret);
}
