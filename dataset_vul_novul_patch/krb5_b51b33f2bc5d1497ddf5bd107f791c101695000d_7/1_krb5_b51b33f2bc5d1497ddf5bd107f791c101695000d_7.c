spnego_gss_export_sec_context(
			    OM_uint32	  *minor_status,
			    gss_ctx_id_t *context_handle,
 			    gss_buffer_t interprocess_token)
 {
 	OM_uint32 ret;
 	ret = gss_export_sec_context(minor_status,
				    context_handle,
 				    interprocess_token);
 	return (ret);
 }
