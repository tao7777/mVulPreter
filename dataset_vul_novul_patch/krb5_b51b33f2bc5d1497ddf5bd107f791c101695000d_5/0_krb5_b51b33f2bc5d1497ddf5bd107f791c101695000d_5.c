spnego_gss_context_time(
			OM_uint32	*minor_status,
			const gss_ctx_id_t context_handle,
 			OM_uint32	*time_rec)
 {
 	OM_uint32 ret;
	spnego_gss_ctx_id_t sc = (spnego_gss_ctx_id_t)context_handle;

	if (sc->ctx_handle == GSS_C_NO_CONTEXT)
		return (GSS_S_NO_CONTEXT);

 	ret = gss_context_time(minor_status,
			    sc->ctx_handle,
 			    time_rec);
 	return (ret);
 }
