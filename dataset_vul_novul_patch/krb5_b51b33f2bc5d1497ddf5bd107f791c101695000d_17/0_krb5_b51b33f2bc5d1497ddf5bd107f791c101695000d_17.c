spnego_gss_set_sec_context_option(
		OM_uint32 *minor_status,
		gss_ctx_id_t *context_handle,
		const gss_OID desired_object,
 		const gss_buffer_t value)
 {
 	OM_uint32 ret;
	spnego_gss_ctx_id_t sc = (spnego_gss_ctx_id_t)*context_handle;

	/* There are no SPNEGO-specific OIDs for this function, and we cannot
	 * construct an empty SPNEGO context with it. */
	if (sc == NULL || sc->ctx_handle == GSS_C_NO_CONTEXT)
		return (GSS_S_UNAVAILABLE);

 	ret = gss_set_sec_context_option(minor_status,
			    &sc->ctx_handle,
 			    desired_object,
 			    value);
 	return (ret);
}
