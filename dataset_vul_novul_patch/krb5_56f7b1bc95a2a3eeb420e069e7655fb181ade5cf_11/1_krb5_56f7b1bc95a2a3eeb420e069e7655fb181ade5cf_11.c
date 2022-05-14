gss_unwrap (minor_status,
            context_handle,
            input_message_buffer,
            output_message_buffer,
            conf_state,
            qop_state)

OM_uint32 *		minor_status;
gss_ctx_id_t		context_handle;
gss_buffer_t		input_message_buffer;
gss_buffer_t		output_message_buffer;
int *			conf_state;
gss_qop_t *		qop_state;

{
/* EXPORT DELETE START */
    OM_uint32		status;
    gss_union_ctx_id_t	ctx;
    gss_mechanism	mech;

    if (minor_status != NULL)
	*minor_status = 0;

    if (output_message_buffer != GSS_C_NO_BUFFER) {
	output_message_buffer->length = 0;
	output_message_buffer->value = NULL;
    }

    if (minor_status == NULL)
	return (GSS_S_CALL_INACCESSIBLE_WRITE);

    if (context_handle == GSS_C_NO_CONTEXT)
	return (GSS_S_CALL_INACCESSIBLE_READ | GSS_S_NO_CONTEXT);

    if (input_message_buffer == GSS_C_NO_BUFFER ||
	GSS_EMPTY_BUFFER(input_message_buffer))

	return (GSS_S_CALL_INACCESSIBLE_READ);

    if (output_message_buffer == GSS_C_NO_BUFFER)
	return (GSS_S_CALL_INACCESSIBLE_WRITE);

    /*
     * select the approprate underlying mechanism routine and
      * call it.
      */
     ctx = (gss_union_ctx_id_t) context_handle;
     mech = gssint_get_mechanism (ctx->mech_type);
 
     if (mech) {
	if (mech->gss_unwrap) {
	    status = mech->gss_unwrap(minor_status,
				      ctx->internal_ctx_id,
				      input_message_buffer,
				      output_message_buffer,
				      conf_state,
				      qop_state);
	    if (status != GSS_S_COMPLETE)
		map_error(minor_status, mech);
	} else if (mech->gss_unwrap_aead || mech->gss_unwrap_iov) {
	    status = gssint_unwrap_aead(mech,
					minor_status,
					ctx,
					input_message_buffer,
					GSS_C_NO_BUFFER,
					output_message_buffer,
					conf_state,
					(gss_qop_t *)qop_state);
	} else
	    status = GSS_S_UNAVAILABLE;

	return(status);
    }

/* EXPORT DELETE END */

    return (GSS_S_BAD_MECH);
}
