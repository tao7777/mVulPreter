spnego_gss_export_sec_context(
			    OM_uint32	  *minor_status,
			    gss_ctx_id_t *context_handle,
 			    gss_buffer_t interprocess_token)
 {
 	OM_uint32 ret;
	spnego_gss_ctx_id_t sc = *(spnego_gss_ctx_id_t *)context_handle;

	/* We don't currently support exporting partially established
	 * contexts. */
	if (!sc->opened)
		return GSS_S_UNAVAILABLE;

 	ret = gss_export_sec_context(minor_status,
				    &sc->ctx_handle,
 				    interprocess_token);
	if (sc->ctx_handle == GSS_C_NO_CONTEXT) {
		release_spnego_ctx(&sc);
		*context_handle = GSS_C_NO_CONTEXT;
	}
 	return (ret);
 }
