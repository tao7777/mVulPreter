spnego_gss_verify_mic(
		OM_uint32 *minor_status,
		const gss_ctx_id_t context_handle,
		const gss_buffer_t msg_buffer,
		const gss_buffer_t token_buffer,
 		gss_qop_t *qop_state)
 {
 	OM_uint32 ret;
	spnego_gss_ctx_id_t sc = (spnego_gss_ctx_id_t)context_handle;

	if (sc->ctx_handle == GSS_C_NO_CONTEXT)
		return (GSS_S_NO_CONTEXT);

 	ret = gss_verify_mic(minor_status,
			    sc->ctx_handle,
 			    msg_buffer,
 			    token_buffer,
 			    qop_state);
	return (ret);
}
