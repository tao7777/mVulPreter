create_spnego_ctx(void)
 {
 	spnego_gss_ctx_id_t spnego_ctx = NULL;
 	spnego_ctx = (spnego_gss_ctx_id_t)
		malloc(sizeof (spnego_gss_ctx_id_rec));

	if (spnego_ctx == NULL) {
		return (NULL);
	}

	spnego_ctx->magic_num = SPNEGO_MAGIC_ID;
	spnego_ctx->ctx_handle = GSS_C_NO_CONTEXT;
	spnego_ctx->mech_set = NULL;
	spnego_ctx->internal_mech = NULL;
	spnego_ctx->optionStr = NULL;
	spnego_ctx->DER_mechTypes.length = 0;
	spnego_ctx->DER_mechTypes.value = NULL;
	spnego_ctx->default_cred = GSS_C_NO_CREDENTIAL;
	spnego_ctx->mic_reqd = 0;
	spnego_ctx->mic_sent = 0;
 	spnego_ctx->mic_rcvd = 0;
 	spnego_ctx->mech_complete = 0;
 	spnego_ctx->nego_done = 0;
 	spnego_ctx->internal_name = GSS_C_NO_NAME;
 	spnego_ctx->actual_mech = GSS_C_NO_OID;
 
	check_spnego_options(spnego_ctx);

	return (spnego_ctx);
}
