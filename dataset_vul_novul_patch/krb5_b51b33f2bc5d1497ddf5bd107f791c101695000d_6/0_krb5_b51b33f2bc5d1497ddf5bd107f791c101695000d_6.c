spnego_gss_delete_sec_context(
			    OM_uint32 *minor_status,
			    gss_ctx_id_t *context_handle,
			    gss_buffer_t output_token)
{
	OM_uint32 ret = GSS_S_COMPLETE;
	spnego_gss_ctx_id_t *ctx =
		    (spnego_gss_ctx_id_t *)context_handle;

	*minor_status = 0;

	if (context_handle == NULL)
		return (GSS_S_FAILURE);

 	if (*ctx == NULL)
 		return (GSS_S_COMPLETE);
 
	(void) gss_delete_sec_context(minor_status, &(*ctx)->ctx_handle,
				      output_token);
	(void) release_spnego_ctx(ctx);
 
 	return (ret);
 }
