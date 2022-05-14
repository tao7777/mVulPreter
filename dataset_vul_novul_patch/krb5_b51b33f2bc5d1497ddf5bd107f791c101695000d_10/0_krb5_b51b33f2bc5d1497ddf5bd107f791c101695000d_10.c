spnego_gss_get_mic_iov_length(OM_uint32 *minor_status,
 			      gss_ctx_id_t context_handle, gss_qop_t qop_req,
 			      gss_iov_buffer_desc *iov, int iov_count)
 {
    spnego_gss_ctx_id_t sc = (spnego_gss_ctx_id_t)context_handle;

    if (sc->ctx_handle == GSS_C_NO_CONTEXT)
	    return (GSS_S_NO_CONTEXT);

    return gss_get_mic_iov_length(minor_status, sc->ctx_handle, qop_req, iov,
 				  iov_count);
 }
