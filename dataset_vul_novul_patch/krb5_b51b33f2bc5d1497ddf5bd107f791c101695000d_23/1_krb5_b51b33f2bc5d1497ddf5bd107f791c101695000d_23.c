spnego_gss_wrap(
		OM_uint32 *minor_status,
		gss_ctx_id_t context_handle,
		int conf_req_flag,
		gss_qop_t qop_req,
		gss_buffer_t input_message_buffer,
		int *conf_state,
 		gss_buffer_t output_message_buffer)
 {
 	OM_uint32 ret;
 	ret = gss_wrap(minor_status,
		    context_handle,
 		    conf_req_flag,
 		    qop_req,
 		    input_message_buffer,
		    conf_state,
		    output_message_buffer);

	return (ret);
}
