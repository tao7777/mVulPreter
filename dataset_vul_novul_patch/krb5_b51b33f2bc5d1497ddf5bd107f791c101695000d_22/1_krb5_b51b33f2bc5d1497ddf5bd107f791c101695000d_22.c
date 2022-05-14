spnego_gss_verify_mic_iov(OM_uint32 *minor_status, gss_ctx_id_t context_handle,
 			  gss_qop_t *qop_state, gss_iov_buffer_desc *iov,
 			  int iov_count)
 {
    return gss_verify_mic_iov(minor_status, context_handle, qop_state, iov,
 			      iov_count);
 }
