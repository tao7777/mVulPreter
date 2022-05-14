init_ctx_new(OM_uint32 *minor_status,
	     spnego_gss_cred_id_t spcred,
	     gss_ctx_id_t *ctx,
	     send_token_flag *tokflag)
{
 	OM_uint32 ret;
 	spnego_gss_ctx_id_t sc = NULL;
 
	sc = create_spnego_ctx();
 	if (sc == NULL)
 		return GSS_S_FAILURE;
 
	/* determine negotiation mech set */
	ret = get_negotiable_mechs(minor_status, spcred, GSS_C_INITIATE,
				   &sc->mech_set);
	if (ret != GSS_S_COMPLETE)
		goto cleanup;

	/* Set an initial internal mech to make the first context token. */
	sc->internal_mech = &sc->mech_set->elements[0];

	if (put_mech_set(sc->mech_set, &sc->DER_mechTypes) < 0) {
 		ret = GSS_S_FAILURE;
 		goto cleanup;
 	}
	/*
	 * The actual context is not yet determined, set the output
	 * context handle to refer to the spnego context itself.
	 */
 	sc->ctx_handle = GSS_C_NO_CONTEXT;
 	*ctx = (gss_ctx_id_t)sc;
 	sc = NULL;
	*tokflag = INIT_TOKEN_SEND;
	ret = GSS_S_CONTINUE_NEEDED;

cleanup:
	release_spnego_ctx(&sc);
	return ret;
}
