spnego_gss_unwrap(
		OM_uint32 *minor_status,
		gss_ctx_id_t context_handle,
		gss_buffer_t input_message_buffer,
		gss_buffer_t output_message_buffer,
		int *conf_state,
 		gss_qop_t *qop_state)
 {
 	OM_uint32 ret;
	spnego_gss_ctx_id_t sc = (spnego_gss_ctx_id_t)context_handle;

	if (sc->ctx_handle == GSS_C_NO_CONTEXT)
		return (GSS_S_NO_CONTEXT);

 	ret = gss_unwrap(minor_status,
			sc->ctx_handle,
 			input_message_buffer,
 			output_message_buffer,
 			conf_state,
			qop_state);

	return (ret);
}
