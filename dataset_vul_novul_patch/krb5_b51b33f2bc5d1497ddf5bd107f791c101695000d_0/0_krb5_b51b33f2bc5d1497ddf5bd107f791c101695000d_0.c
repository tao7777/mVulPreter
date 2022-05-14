acc_ctx_new(OM_uint32 *minor_status,
	    gss_buffer_t buf,
	    gss_ctx_id_t *ctx,
	    spnego_gss_cred_id_t spcred,
	    gss_buffer_t *mechToken,
	    gss_buffer_t *mechListMIC,
	    OM_uint32 *negState,
	    send_token_flag *return_token)
{
	OM_uint32 tmpmin, ret, req_flags;
	gss_OID_set supported_mechSet, mechTypes;
	gss_buffer_desc der_mechTypes;
	gss_OID mech_wanted;
	spnego_gss_ctx_id_t sc = NULL;

	ret = GSS_S_DEFECTIVE_TOKEN;
	der_mechTypes.length = 0;
	der_mechTypes.value = NULL;
	*mechToken = *mechListMIC = GSS_C_NO_BUFFER;
	supported_mechSet = mechTypes = GSS_C_NO_OID_SET;
	*return_token = ERROR_TOKEN_SEND;
	*negState = REJECT;
	*minor_status = 0;

	ret = get_negTokenInit(minor_status, buf, &der_mechTypes,
			       &mechTypes, &req_flags,
			       mechToken, mechListMIC);
	if (ret != GSS_S_COMPLETE) {
		goto cleanup;
	}
	ret = get_negotiable_mechs(minor_status, spcred, GSS_C_ACCEPT,
				   &supported_mechSet);
	if (ret != GSS_S_COMPLETE) {
		*return_token = NO_TOKEN_SEND;
		goto cleanup;
	}
	/*
	 * Select the best match between the list of mechs
	 * that the initiator requested and the list that
	 * the acceptor will support.
	 */
	mech_wanted = negotiate_mech(supported_mechSet, mechTypes, negState);
	if (*negState == REJECT) {
		ret = GSS_S_BAD_MECH;
		goto cleanup;
	}
	sc = (spnego_gss_ctx_id_t)*ctx;
	if (sc != NULL) {
 		gss_release_buffer(&tmpmin, &sc->DER_mechTypes);
 		assert(mech_wanted != GSS_C_NO_OID);
 	} else
		sc = create_spnego_ctx(0);
 	if (sc == NULL) {
 		ret = GSS_S_FAILURE;
 		*return_token = NO_TOKEN_SEND;
		goto cleanup;
	}
	sc->mech_set = mechTypes;
	mechTypes = GSS_C_NO_OID_SET;
	sc->internal_mech = mech_wanted;
	sc->DER_mechTypes = der_mechTypes;
	der_mechTypes.length = 0;
	der_mechTypes.value = NULL;

	if (*negState == REQUEST_MIC)
		sc->mic_reqd = 1;

	*return_token = INIT_TOKEN_SEND;
	sc->firstpass = 1;
	*ctx = (gss_ctx_id_t)sc;
	ret = GSS_S_COMPLETE;
cleanup:
	gss_release_oid_set(&tmpmin, &mechTypes);
	gss_release_oid_set(&tmpmin, &supported_mechSet);
	if (der_mechTypes.length != 0)
		gss_release_buffer(&tmpmin, &der_mechTypes);

	return ret;
}
