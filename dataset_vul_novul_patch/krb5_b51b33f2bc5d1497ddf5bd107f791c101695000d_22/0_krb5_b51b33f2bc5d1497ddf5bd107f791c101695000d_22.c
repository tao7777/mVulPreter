spnego_gss_verify_mic_iov(OM_uint32 *minor_status, gss_ctx_id_t context_handle,
 			  gss_qop_t *qop_state, gss_iov_buffer_desc *iov,
 			  int iov_count)
 {
    spnego_gss_ctx_id_t sc = (spnego_gss_ctx_id_t)context_handle;

    if (sc->ctx_handle == GSS_C_NO_CONTEXT)
	    return (GSS_S_NO_CONTEXT);

    return gss_verify_mic_iov(minor_status, sc->ctx_handle, qop_state, iov,
 			      iov_count);
 }
