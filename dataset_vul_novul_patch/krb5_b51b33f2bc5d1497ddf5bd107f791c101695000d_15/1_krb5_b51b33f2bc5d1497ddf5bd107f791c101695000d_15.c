spnego_gss_process_context_token(
				OM_uint32	*minor_status,
				const gss_ctx_id_t context_handle,
 				const gss_buffer_t token_buffer)
 {
 	OM_uint32 ret;
 	ret = gss_process_context_token(minor_status,
					context_handle,
 					token_buffer);
 
 	return (ret);
}
