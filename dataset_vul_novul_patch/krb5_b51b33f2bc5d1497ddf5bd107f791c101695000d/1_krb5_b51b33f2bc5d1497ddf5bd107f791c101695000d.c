acc_ctx_hints(OM_uint32 *minor_status,
	      gss_ctx_id_t *ctx,
	      spnego_gss_cred_id_t spcred,
	      gss_buffer_t *mechListMIC,
	      OM_uint32 *negState,
	      send_token_flag *return_token)
{
	OM_uint32 tmpmin, ret;
	gss_OID_set supported_mechSet;
	spnego_gss_ctx_id_t sc = NULL;

	*mechListMIC = GSS_C_NO_BUFFER;
	supported_mechSet = GSS_C_NO_OID_SET;
	*return_token = NO_TOKEN_SEND;
	*negState = REJECT;
	*minor_status = 0;

	/* A hint request must be the first token received. */
	if (*ctx != GSS_C_NO_CONTEXT)
	    return GSS_S_DEFECTIVE_TOKEN;

	ret = get_negotiable_mechs(minor_status, spcred, GSS_C_ACCEPT,
				   &supported_mechSet);
	if (ret != GSS_S_COMPLETE)
		goto cleanup;

	ret = make_NegHints(minor_status, mechListMIC);
 	if (ret != GSS_S_COMPLETE)
 		goto cleanup;
 
	sc = create_spnego_ctx();
 	if (sc == NULL) {
 		ret = GSS_S_FAILURE;
 		goto cleanup;
	}
	if (put_mech_set(supported_mechSet, &sc->DER_mechTypes) < 0) {
		ret = GSS_S_FAILURE;
		goto cleanup;
	}
	sc->internal_mech = GSS_C_NO_OID;

	*negState = ACCEPT_INCOMPLETE;
	*return_token = INIT_TOKEN_SEND;
	sc->firstpass = 1;
	*ctx = (gss_ctx_id_t)sc;
	sc = NULL;
	ret = GSS_S_COMPLETE;

cleanup:
	release_spnego_ctx(&sc);
	gss_release_oid_set(&tmpmin, &supported_mechSet);

	return ret;
}
