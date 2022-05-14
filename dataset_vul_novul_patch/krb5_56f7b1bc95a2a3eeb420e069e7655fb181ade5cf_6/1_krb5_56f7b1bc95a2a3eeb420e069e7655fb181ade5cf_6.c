gss_pseudo_random (OM_uint32 *minor_status,
	           gss_ctx_id_t context_handle,
	           int prf_key,
	           const gss_buffer_t prf_in,
	           ssize_t desired_output_len,
	           gss_buffer_t prf_out)
{
    OM_uint32		status;
    gss_union_ctx_id_t	ctx;
    gss_mechanism	mech;

    if (minor_status == NULL)
	return GSS_S_CALL_INACCESSIBLE_WRITE;

    if (context_handle == GSS_C_NO_CONTEXT)
	return GSS_S_CALL_INACCESSIBLE_READ | GSS_S_NO_CONTEXT;

    if (prf_in == GSS_C_NO_BUFFER)
	return GSS_S_CALL_INACCESSIBLE_READ | GSS_S_NO_CONTEXT;

    if (prf_out == GSS_C_NO_BUFFER)
	return GSS_S_CALL_INACCESSIBLE_WRITE | GSS_S_NO_CONTEXT;

    prf_out->length = 0;
    prf_out->value = NULL;

    /*
     * select the approprate underlying mechanism routine and
     * call it.
      */
 
     ctx = (gss_union_ctx_id_t) context_handle;
     mech = gssint_get_mechanism (ctx->mech_type);
 
     if (mech != NULL) {
	if (mech->gss_pseudo_random != NULL) {
	    status = mech->gss_pseudo_random(minor_status,
					     ctx->internal_ctx_id,
					     prf_key,
					     prf_in,
					     desired_output_len,
					     prf_out);
	    if (status != GSS_S_COMPLETE)
		map_error(minor_status, mech);
	} else
	    status = GSS_S_UNAVAILABLE;

	return status;
    }

    return GSS_S_BAD_MECH;
}
