spnego_gss_unwrap_aead(OM_uint32 *minor_status,
		       gss_ctx_id_t context_handle,
		       gss_buffer_t input_message_buffer,
		       gss_buffer_t input_assoc_buffer,
		       gss_buffer_t output_payload_buffer,
		       int *conf_state,
 		       gss_qop_t *qop_state)
 {
 	OM_uint32 ret;
 	ret = gss_unwrap_aead(minor_status,
			      context_handle,
 			      input_message_buffer,
 			      input_assoc_buffer,
 			      output_payload_buffer,
			      conf_state,
			      qop_state);
	return (ret);
}
