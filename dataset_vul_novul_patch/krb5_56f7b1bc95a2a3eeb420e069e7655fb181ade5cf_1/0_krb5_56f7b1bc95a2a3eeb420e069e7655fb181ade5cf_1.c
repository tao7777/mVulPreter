gss_context_time (minor_status,
                  context_handle,
                  time_rec)

OM_uint32 *		minor_status;
gss_ctx_id_t		context_handle;
OM_uint32 *		time_rec;

{
    OM_uint32		status;
    gss_union_ctx_id_t	ctx;
    gss_mechanism	mech;

    if (minor_status == NULL)
	return (GSS_S_CALL_INACCESSIBLE_WRITE);
    *minor_status = 0;

    if (time_rec == NULL)
	return (GSS_S_CALL_INACCESSIBLE_WRITE);

    if (context_handle == GSS_C_NO_CONTEXT)
	return (GSS_S_CALL_INACCESSIBLE_READ | GSS_S_NO_CONTEXT);

    /*
     * select the approprate underlying mechanism routine and
     * call it.
      */
 
     ctx = (gss_union_ctx_id_t) context_handle;
    if (ctx->internal_ctx_id == GSS_C_NO_CONTEXT)
	return (GSS_S_NO_CONTEXT);
     mech = gssint_get_mechanism (ctx->mech_type);
 
     if (mech) {

	if (mech->gss_context_time) {
	    status = mech->gss_context_time(
					    minor_status,
					    ctx->internal_ctx_id,
					    time_rec);
	    if (status != GSS_S_COMPLETE)
		map_error(minor_status, mech);
	} else
	    status = GSS_S_UNAVAILABLE;

	return(status);
    }

    return (GSS_S_BAD_MECH);
}
