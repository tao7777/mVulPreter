gss_wrap_iov_length (minor_status,
                     context_handle,
                     conf_req_flag,
                     qop_req,
                     conf_state,
                     iov,
                     iov_count)
OM_uint32 *		minor_status;
gss_ctx_id_t		context_handle;
int			conf_req_flag;
gss_qop_t		qop_req;
int *			conf_state;
gss_iov_buffer_desc  *	iov;
int			iov_count;
{
 /* EXPORT DELETE START */

    OM_uint32		status;
    gss_union_ctx_id_t	ctx;
    gss_mechanism	mech;

    status = val_wrap_iov_args(minor_status, context_handle,
			       conf_req_flag, qop_req,
			       conf_state, iov, iov_count);
    if (status != GSS_S_COMPLETE)
	return (status);

    /*
     * select the approprate underlying mechanism routine and
     * call it.
      */
 
     ctx = (gss_union_ctx_id_t) context_handle;
     mech = gssint_get_mechanism (ctx->mech_type);
 
     if (mech) {
	if (mech->gss_wrap_iov_length) {
	    status = mech->gss_wrap_iov_length(
					      minor_status,
					      ctx->internal_ctx_id,
					      conf_req_flag,
					      qop_req,
					      conf_state,
					      iov,
					      iov_count);
	    if (status != GSS_S_COMPLETE)
		map_error(minor_status, mech);
	} else
	    status = GSS_S_UNAVAILABLE;

	return(status);
    }
 /* EXPORT DELETE END */

    return (GSS_S_BAD_MECH);
}
