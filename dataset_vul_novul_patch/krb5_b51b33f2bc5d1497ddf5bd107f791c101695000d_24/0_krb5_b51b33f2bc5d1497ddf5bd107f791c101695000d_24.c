spnego_gss_wrap_aead(OM_uint32 *minor_status,
		     gss_ctx_id_t context_handle,
		     int conf_req_flag,
		     gss_qop_t qop_req,
		     gss_buffer_t input_assoc_buffer,
		     gss_buffer_t input_payload_buffer,
		     int *conf_state,
 		     gss_buffer_t output_message_buffer)
 {
 	OM_uint32 ret;
	spnego_gss_ctx_id_t sc = (spnego_gss_ctx_id_t)context_handle;

	if (sc->ctx_handle == GSS_C_NO_CONTEXT)
		return (GSS_S_NO_CONTEXT);

 	ret = gss_wrap_aead(minor_status,
			    sc->ctx_handle,
 			    conf_req_flag,
 			    qop_req,
 			    input_assoc_buffer,
			    input_payload_buffer,
			    conf_state,
			    output_message_buffer);

	return (ret);
}
