spnego_gss_accept_sec_context(
			    OM_uint32 *minor_status,
			    gss_ctx_id_t *context_handle,
			    gss_cred_id_t verifier_cred_handle,
			    gss_buffer_t input_token,
			    gss_channel_bindings_t input_chan_bindings,
			    gss_name_t *src_name,
			    gss_OID *mech_type,
			    gss_buffer_t output_token,
			    OM_uint32 *ret_flags,
			    OM_uint32 *time_rec,
			    gss_cred_id_t *delegated_cred_handle)
{
	OM_uint32 ret, tmpmin, negState;
	send_token_flag return_token;
	gss_buffer_t mechtok_in, mic_in, mic_out;
	gss_buffer_desc mechtok_out = GSS_C_EMPTY_BUFFER;
	spnego_gss_ctx_id_t sc = NULL;
	spnego_gss_cred_id_t spcred = NULL;
	int sendTokenInit = 0, tmpret;

	mechtok_in = mic_in = mic_out = GSS_C_NO_BUFFER;

	/*
	 * This function works in three steps:
	 *
	 *   1. Perform mechanism negotiation.
	 *   2. Invoke the negotiated mech's gss_accept_sec_context function
	 *      and examine the results.
	 *   3. Process or generate MICs if necessary.
	 *
	 * Step one determines whether the negotiation requires a MIC exchange,
	 * while steps two and three share responsibility for determining when
	 * the exchange is complete.  If the selected mech completes in this
	 * call and no MIC exchange is expected, then step 2 will decide.  If a
	 * MIC exchange is expected, then step 3 will decide.  If an error
	 * occurs in any step, the exchange will be aborted, possibly with an
	 * error token.
	 *
	 * negState determines the state of the negotiation, and is
	 * communicated to the acceptor if a continuing token is sent.
	 * return_token is used to indicate what type of token, if any, should
	 * be generated.
	 */

	/* Validate arguments. */
	if (minor_status != NULL)
		*minor_status = 0;
	if (output_token != GSS_C_NO_BUFFER) {
		output_token->length = 0;
		output_token->value = NULL;
	}

	if (minor_status == NULL ||
	    output_token == GSS_C_NO_BUFFER ||
	    context_handle == NULL)
		return GSS_S_CALL_INACCESSIBLE_WRITE;

	if (input_token == GSS_C_NO_BUFFER)
		return GSS_S_CALL_INACCESSIBLE_READ;

	/* Step 1: Perform mechanism negotiation. */
	sc = (spnego_gss_ctx_id_t)*context_handle;
	spcred = (spnego_gss_cred_id_t)verifier_cred_handle;
	if (sc == NULL || sc->internal_mech == GSS_C_NO_OID) {
		/* Process an initial token or request for NegHints. */
		if (src_name != NULL)
			*src_name = GSS_C_NO_NAME;
		if (mech_type != NULL)
			*mech_type = GSS_C_NO_OID;
		if (time_rec != NULL)
			*time_rec = 0;
		if (ret_flags != NULL)
			*ret_flags = 0;
		if (delegated_cred_handle != NULL)
			*delegated_cred_handle = GSS_C_NO_CREDENTIAL;
		if (input_token->length == 0) {
			ret = acc_ctx_hints(minor_status,
					    context_handle, spcred,
					    &mic_out,
					    &negState,
					    &return_token);
			if (ret != GSS_S_COMPLETE)
				goto cleanup;
			sendTokenInit = 1;
			ret = GSS_S_CONTINUE_NEEDED;
		} else {
			/* Can set negState to REQUEST_MIC */
			ret = acc_ctx_new(minor_status, input_token,
					  context_handle, spcred,
					  &mechtok_in, &mic_in,
					  &negState, &return_token);
			if (ret != GSS_S_COMPLETE)
				goto cleanup;
			ret = GSS_S_CONTINUE_NEEDED;
		}
	} else {
		/* Process a response token.  Can set negState to
		 * ACCEPT_INCOMPLETE. */
		ret = acc_ctx_cont(minor_status, input_token,
				   context_handle, &mechtok_in,
				   &mic_in, &negState, &return_token);
		if (ret != GSS_S_COMPLETE)
			goto cleanup;
		ret = GSS_S_CONTINUE_NEEDED;
	}

	/* Step 2: invoke the negotiated mechanism's gss_accept_sec_context
	 * function. */
	sc = (spnego_gss_ctx_id_t)*context_handle;
	/*
	 * Handle mechtok_in and mic_in only if they are
	 * present in input_token.  If neither is present, whether
	 * this is an error depends on whether this is the first
	 * round-trip.  RET is set to a default value according to
	 * whether it is the first round-trip.
	 */
	if (negState != REQUEST_MIC && mechtok_in != GSS_C_NO_BUFFER) {
		ret = acc_ctx_call_acc(minor_status, sc, spcred,
				       mechtok_in, mech_type, &mechtok_out,
				       ret_flags, time_rec,
				       delegated_cred_handle,
				       &negState, &return_token);
	}

	/* Step 3: process or generate the MIC, if the negotiated mech is
	 * complete and supports MICs. */
	if (!HARD_ERROR(ret) && sc->mech_complete &&
	    (sc->ctx_flags & GSS_C_INTEG_FLAG)) {

		ret = handle_mic(minor_status, mic_in,
				 (mechtok_out.length != 0),
				 sc, &mic_out,
				 &negState, &return_token);
	}
cleanup:
	if (return_token == INIT_TOKEN_SEND && sendTokenInit) {
		assert(sc != NULL);
		tmpret = make_spnego_tokenInit_msg(sc, 1, mic_out, 0,
						   GSS_C_NO_BUFFER,
						   return_token, output_token);
		if (tmpret < 0)
			ret = GSS_S_FAILURE;
	} else if (return_token != NO_TOKEN_SEND &&
		   return_token != CHECK_MIC) {
		tmpret = make_spnego_tokenTarg_msg(negState,
						   sc ? sc->internal_mech :
						   GSS_C_NO_OID,
						   &mechtok_out, mic_out,
						   return_token,
						   output_token);
		if (tmpret < 0)
 			ret = GSS_S_FAILURE;
 	}
 	if (ret == GSS_S_COMPLETE) {
		sc->opened = 1;
 		if (sc->internal_name != GSS_C_NO_NAME &&
 		    src_name != NULL) {
 			*src_name = sc->internal_name;
 			sc->internal_name = GSS_C_NO_NAME;
 		}
 	} else if (ret != GSS_S_CONTINUE_NEEDED) {
 		if (sc != NULL) {
 			gss_delete_sec_context(&tmpmin, &sc->ctx_handle,
					       GSS_C_NO_BUFFER);
			release_spnego_ctx(&sc);
		}
		*context_handle = GSS_C_NO_CONTEXT;
	}
	gss_release_buffer(&tmpmin, &mechtok_out);
	if (mechtok_in != GSS_C_NO_BUFFER) {
		gss_release_buffer(&tmpmin, mechtok_in);
		free(mechtok_in);
	}
	if (mic_in != GSS_C_NO_BUFFER) {
		gss_release_buffer(&tmpmin, mic_in);
		free(mic_in);
	}
	if (mic_out != GSS_C_NO_BUFFER) {
		gss_release_buffer(&tmpmin, mic_out);
		free(mic_out);
	}
	return ret;
}
