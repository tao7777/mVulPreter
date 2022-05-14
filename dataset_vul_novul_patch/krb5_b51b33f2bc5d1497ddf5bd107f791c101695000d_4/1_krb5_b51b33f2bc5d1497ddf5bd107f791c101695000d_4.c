spnego_gss_complete_auth_token(
		OM_uint32 *minor_status,
		const gss_ctx_id_t context_handle,
 		gss_buffer_t input_message_buffer)
 {
 	OM_uint32 ret;
 	ret = gss_complete_auth_token(minor_status,
				      context_handle,
 				      input_message_buffer);
 	return (ret);
 }
