spnego_gss_wrap_size_limit(
	OM_uint32	*minor_status,
	const gss_ctx_id_t context_handle,
	int		conf_req_flag,
	gss_qop_t	qop_req,
	OM_uint32	req_output_size,
 	OM_uint32	*max_input_size)
 {
 	OM_uint32 ret;
 	ret = gss_wrap_size_limit(minor_status,
				context_handle,
 				conf_req_flag,
 				qop_req,
 				req_output_size,
				max_input_size);
	return (ret);
}
