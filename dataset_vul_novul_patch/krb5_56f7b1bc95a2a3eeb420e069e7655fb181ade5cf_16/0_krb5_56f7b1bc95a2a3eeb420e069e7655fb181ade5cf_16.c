gss_wrap_aead (minor_status,
               context_handle,
               conf_req_flag,
               qop_req,
	       input_assoc_buffer,
	       input_payload_buffer,
               conf_state,
               output_message_buffer)
OM_uint32 *		minor_status;
gss_ctx_id_t		context_handle;
int			conf_req_flag;
gss_qop_t		qop_req;
gss_buffer_t		input_assoc_buffer;
gss_buffer_t		input_payload_buffer;
int *			conf_state;
gss_buffer_t		output_message_buffer;
{
    OM_uint32		status;
    gss_mechanism	mech;
    gss_union_ctx_id_t	ctx;

    status = val_wrap_aead_args(minor_status, context_handle,
				conf_req_flag, qop_req,
				input_assoc_buffer, input_payload_buffer,
				conf_state, output_message_buffer);
    if (status != GSS_S_COMPLETE)
	return (status);

    /*
     * select the approprate underlying mechanism routine and
      * call it.
      */
     ctx = (gss_union_ctx_id_t)context_handle;
    if (ctx->internal_ctx_id == GSS_C_NO_CONTEXT)
	return (GSS_S_NO_CONTEXT);
     mech = gssint_get_mechanism (ctx->mech_type);
     if (!mech)
 	return (GSS_S_BAD_MECH);

    return gssint_wrap_aead(mech, minor_status, ctx,
			    conf_req_flag, qop_req,
			    input_assoc_buffer, input_payload_buffer,
			    conf_state, output_message_buffer);
}
