gss_export_sec_context(minor_status,
                       context_handle,
                       interprocess_token)

OM_uint32 *		minor_status;
gss_ctx_id_t *		context_handle;
gss_buffer_t		interprocess_token;

{
    OM_uint32		status;
    OM_uint32 		length;
    gss_union_ctx_id_t	ctx = NULL;
    gss_mechanism	mech;
    gss_buffer_desc	token = GSS_C_EMPTY_BUFFER;
    char		*buf;

    status = val_exp_sec_ctx_args(minor_status,
				  context_handle, interprocess_token);
    if (status != GSS_S_COMPLETE)
	return (status);

    /*
     * select the approprate underlying mechanism routine and
     * call it.
      */
 
     ctx = (gss_union_ctx_id_t) *context_handle;
     mech = gssint_get_mechanism (ctx->mech_type);
     if (!mech)
 	return GSS_S_BAD_MECH;
    if (!mech->gss_export_sec_context)
	return (GSS_S_UNAVAILABLE);

    status = mech->gss_export_sec_context(minor_status,
					  &ctx->internal_ctx_id, &token);
    if (status != GSS_S_COMPLETE) {
	map_error(minor_status, mech);
	goto cleanup;
    }

    length = token.length + 4 + ctx->mech_type->length;
    interprocess_token->length = length;
    interprocess_token->value = malloc(length);
    if (interprocess_token->value == 0) {
	*minor_status = ENOMEM;
	status = GSS_S_FAILURE;
	goto cleanup;
    }
    buf = interprocess_token->value;
    length = ctx->mech_type->length;
    buf[3] = (unsigned char) (length & 0xFF);
    length >>= 8;
    buf[2] = (unsigned char) (length & 0xFF);
    length >>= 8;
    buf[1] = (unsigned char) (length & 0xFF);
    length >>= 8;
    buf[0] = (unsigned char) (length & 0xFF);
    memcpy(buf+4, ctx->mech_type->elements, (size_t) ctx->mech_type->length);
    memcpy(buf+4+ctx->mech_type->length, token.value, token.length);

    status = GSS_S_COMPLETE;

cleanup:
    (void) gss_release_buffer(minor_status, &token);
    if (ctx != NULL && ctx->internal_ctx_id == GSS_C_NO_CONTEXT) {
	/* If the mech deleted its context, delete the union context. */
	free(ctx->mech_type->elements);
	free(ctx->mech_type);
	free(ctx);
	*context_handle = GSS_C_NO_CONTEXT;
    }
    return status;
}
