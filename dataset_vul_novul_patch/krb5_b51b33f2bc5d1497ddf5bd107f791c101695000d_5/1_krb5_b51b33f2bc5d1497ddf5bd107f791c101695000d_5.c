spnego_gss_context_time(
			OM_uint32	*minor_status,
			const gss_ctx_id_t context_handle,
 			OM_uint32	*time_rec)
 {
 	OM_uint32 ret;
 	ret = gss_context_time(minor_status,
			    context_handle,
 			    time_rec);
 	return (ret);
 }
