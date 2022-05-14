gss_get_mic (minor_status,
	     context_handle,
	     qop_req,
	     message_buffer,
	     msg_token)

OM_uint32 *		minor_status;
gss_ctx_id_t		context_handle;
gss_qop_t		qop_req;
gss_buffer_t		message_buffer;
gss_buffer_t		msg_token;

{
    OM_uint32		status;
    gss_union_ctx_id_t	ctx;
    gss_mechanism	mech;

    status = val_get_mic_args(minor_status, context_handle,
			      qop_req, message_buffer, msg_token);
    if (status != GSS_S_COMPLETE)
	return (status);

    /*
     * select the approprate underlying mechanism routine and
     * call it.
      */
 
     ctx = (gss_union_ctx_id_t) context_handle;
    if (ctx->internal_ctx_id == GSS_C_NO_CONTEXT)
	return (GSS_S_NO_CONTEXT);
     mech = gssint_get_mechanism (ctx->mech_type);
 
     if (mech) {
	if (mech->gss_get_mic) {
	    status = mech->gss_get_mic(
				    minor_status,
				    ctx->internal_ctx_id,
				    qop_req,
				    message_buffer,
				    msg_token);
	    if (status != GSS_S_COMPLETE)
		map_error(minor_status, mech);
	} else
	    status = GSS_S_UNAVAILABLE;

	return(status);
    }

    return (GSS_S_BAD_MECH);
}
