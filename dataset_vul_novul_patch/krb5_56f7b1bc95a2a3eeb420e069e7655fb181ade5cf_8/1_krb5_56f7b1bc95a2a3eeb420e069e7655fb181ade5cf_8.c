gss_wrap( OM_uint32 *minor_status,
          gss_ctx_id_t context_handle,
          int conf_req_flag,
          gss_qop_t qop_req,
          gss_buffer_t input_message_buffer,
          int *conf_state,
          gss_buffer_t output_message_buffer)
{

    /* EXPORT DELETE START */

    OM_uint32           status;
    gss_union_ctx_id_t  ctx;
    gss_mechanism       mech;

    status = val_wrap_args(minor_status, context_handle,
                           conf_req_flag, qop_req,
                           input_message_buffer, conf_state,
                           output_message_buffer);
    if (status != GSS_S_COMPLETE)
        return (status);

    /*
     * select the approprate underlying mechanism routine and
     * call it.
      */
 
     ctx = (gss_union_ctx_id_t) context_handle;
     mech = gssint_get_mechanism (ctx->mech_type);
 
     if (mech) {
        if (mech->gss_wrap) {
            status = mech->gss_wrap(minor_status,
                                    ctx->internal_ctx_id,
                                    conf_req_flag,
                                    qop_req,
                                    input_message_buffer,
                                    conf_state,
                                    output_message_buffer);
            if (status != GSS_S_COMPLETE)
                map_error(minor_status, mech);
        } else if (mech->gss_wrap_aead ||
                   (mech->gss_wrap_iov && mech->gss_wrap_iov_length)) {
            status = gssint_wrap_aead(mech,
                                      minor_status,
                                      ctx,
                                      conf_req_flag,
                                      (gss_qop_t)qop_req,
                                      GSS_C_NO_BUFFER,
                                      input_message_buffer,
                                      conf_state,
                                      output_message_buffer);
        } else
            status = GSS_S_UNAVAILABLE;

        return(status);
    }
    /* EXPORT DELETE END */

    return (GSS_S_BAD_MECH);
}
