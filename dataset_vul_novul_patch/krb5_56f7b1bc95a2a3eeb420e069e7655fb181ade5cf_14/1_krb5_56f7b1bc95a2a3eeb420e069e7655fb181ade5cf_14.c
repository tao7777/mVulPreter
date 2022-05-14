gss_verify_mic_iov(OM_uint32 *minor_status, gss_ctx_id_t context_handle,
		   gss_qop_t *qop_state, gss_iov_buffer_desc *iov,
		   int iov_count)
{
    OM_uint32 status;
    gss_union_ctx_id_t ctx;
    gss_mechanism mech;

    status = val_unwrap_iov_args(minor_status, context_handle, NULL,
				 qop_state, iov, iov_count);
    if (status != GSS_S_COMPLETE)
	return status;
 
     /* Select the approprate underlying mechanism routine and call it. */
     ctx = (gss_union_ctx_id_t)context_handle;
     mech = gssint_get_mechanism(ctx->mech_type);
     if (mech == NULL)
 	return GSS_S_BAD_MECH;
    if (mech->gss_verify_mic_iov == NULL)
	return GSS_S_UNAVAILABLE;
    status = mech->gss_verify_mic_iov(minor_status, ctx->internal_ctx_id,
				      qop_state, iov, iov_count);
    if (status != GSS_S_COMPLETE)
	map_error(minor_status, mech);
    return status;
}
