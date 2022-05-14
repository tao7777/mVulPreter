spnego_gss_set_sec_context_option(
		OM_uint32 *minor_status,
		gss_ctx_id_t *context_handle,
		const gss_OID desired_object,
 		const gss_buffer_t value)
 {
 	OM_uint32 ret;
 	ret = gss_set_sec_context_option(minor_status,
			    context_handle,
 			    desired_object,
 			    value);
 	return (ret);
}
