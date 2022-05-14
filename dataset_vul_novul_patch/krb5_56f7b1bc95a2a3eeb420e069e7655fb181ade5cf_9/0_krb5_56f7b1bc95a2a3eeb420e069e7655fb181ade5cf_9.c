gss_wrap_size_limit(OM_uint32  *minor_status,
                    gss_ctx_id_t context_handle,
                    int conf_req_flag,
                    gss_qop_t qop_req, OM_uint32 req_output_size, OM_uint32 *max_input_size)
{
    gss_union_ctx_id_t  ctx;
    gss_mechanism       mech;
    OM_uint32           major_status;

    if (minor_status == NULL)
        return (GSS_S_CALL_INACCESSIBLE_WRITE);
    *minor_status = 0;

    if (context_handle == GSS_C_NO_CONTEXT)
        return (GSS_S_CALL_INACCESSIBLE_READ | GSS_S_NO_CONTEXT);

    if (max_input_size == NULL)
        return (GSS_S_CALL_INACCESSIBLE_WRITE);

    /*
     * select the approprate underlying mechanism routine and
     * call it.
      */
 
     ctx = (gss_union_ctx_id_t) context_handle;
    if (ctx->internal_ctx_id == GSS_C_NO_CONTEXT)
        return (GSS_S_NO_CONTEXT);
     mech = gssint_get_mechanism (ctx->mech_type);
 
     if (!mech)
        return (GSS_S_BAD_MECH);

    if (mech->gss_wrap_size_limit)
        major_status = mech->gss_wrap_size_limit(minor_status,
                                                 ctx->internal_ctx_id,
                                                 conf_req_flag, qop_req,
                                                 req_output_size, max_input_size);
    else if (mech->gss_wrap_iov_length)
        major_status = gssint_wrap_size_limit_iov_shim(mech, minor_status,
                                                       ctx->internal_ctx_id,
                                                       conf_req_flag, qop_req,
                                                       req_output_size, max_input_size);
    else
        major_status = GSS_S_UNAVAILABLE;
    if (major_status != GSS_S_COMPLETE)
        map_error(minor_status, mech);
    return major_status;
}
