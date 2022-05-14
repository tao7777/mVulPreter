gss_unwrap_aead (minor_status,
                 context_handle,
		 input_message_buffer,
		 input_assoc_buffer,
		 output_payload_buffer,
                 conf_state,
                 qop_state)
OM_uint32 *		minor_status;
gss_ctx_id_t		context_handle;
gss_buffer_t		input_message_buffer;
gss_buffer_t		input_assoc_buffer;
gss_buffer_t		output_payload_buffer;
int 			*conf_state;
gss_qop_t		*qop_state;
{

    OM_uint32		status;
    gss_union_ctx_id_t	ctx;
    gss_mechanism	mech;

    status = val_unwrap_aead_args(minor_status, context_handle,
				  input_message_buffer, input_assoc_buffer,
				  output_payload_buffer,
				  conf_state, qop_state);
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
 
     if (!mech)
	return (GSS_S_BAD_MECH);

    return gssint_unwrap_aead(mech, minor_status, ctx,
			      input_message_buffer, input_assoc_buffer,
			      output_payload_buffer, conf_state, qop_state);
}
