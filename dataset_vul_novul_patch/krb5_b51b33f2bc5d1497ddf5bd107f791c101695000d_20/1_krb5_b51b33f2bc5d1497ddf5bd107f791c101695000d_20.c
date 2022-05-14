spnego_gss_unwrap_iov(OM_uint32 *minor_status,
		      gss_ctx_id_t context_handle,
		      int *conf_state,
		      gss_qop_t *qop_state,
		      gss_iov_buffer_desc *iov,
 		      int iov_count)
 {
 	OM_uint32 ret;
 	ret = gss_unwrap_iov(minor_status,
			     context_handle,
 			     conf_state,
 			     qop_state,
 			     iov,
			     iov_count);
	return (ret);
}
