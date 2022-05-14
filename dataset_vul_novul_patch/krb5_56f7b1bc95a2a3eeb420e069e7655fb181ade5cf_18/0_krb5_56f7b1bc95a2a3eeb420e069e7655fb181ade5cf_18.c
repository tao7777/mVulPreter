gss_get_mic_iov_length(OM_uint32 *minor_status, gss_ctx_id_t context_handle,
		       gss_qop_t qop_req, gss_iov_buffer_desc *iov,
		       int iov_count)
{
    OM_uint32 status;
    gss_union_ctx_id_t ctx;
    gss_mechanism mech;

    status = val_wrap_iov_args(minor_status, context_handle, 0, qop_req, NULL,
			       iov, iov_count);
    if (status != GSS_S_COMPLETE)
	return status;
 
     /* Select the approprate underlying mechanism routine and call it. */
     ctx = (gss_union_ctx_id_t)context_handle;
    if (ctx->internal_ctx_id == GSS_C_NO_CONTEXT)
	return GSS_S_NO_CONTEXT;
     mech = gssint_get_mechanism(ctx->mech_type);
     if (mech == NULL)
 	return GSS_S_BAD_MECH;
    if (mech->gss_get_mic_iov_length == NULL)
	return GSS_S_UNAVAILABLE;
    status = mech->gss_get_mic_iov_length(minor_status, ctx->internal_ctx_id,
					  qop_req, iov, iov_count);
    if (status != GSS_S_COMPLETE)
	map_error(minor_status, mech);
    return status;
}
