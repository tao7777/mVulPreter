spnego_gss_inquire_sec_context_by_oid(
		OM_uint32 *minor_status,
		const gss_ctx_id_t context_handle,
		const gss_OID desired_object,
 		gss_buffer_set_t *data_set)
 {
 	OM_uint32 ret;
	spnego_gss_ctx_id_t sc = (spnego_gss_ctx_id_t)context_handle;

	/* There are no SPNEGO-specific OIDs for this function. */
	if (sc->ctx_handle == GSS_C_NO_CONTEXT)
		return (GSS_S_UNAVAILABLE);

 	ret = gss_inquire_sec_context_by_oid(minor_status,
			    sc->ctx_handle,
 			    desired_object,
 			    data_set);
 	return (ret);
}
