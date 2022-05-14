krb5_init_creds_step(krb5_context context,
		     krb5_init_creds_context ctx,
		     krb5_data *in,
		     krb5_data *out,
		     krb5_krbhst_info *hostinfo,
		     unsigned int *flags)
{
    krb5_error_code ret;
    size_t len = 0;
    size_t size;
    AS_REQ req2;

    krb5_data_zero(out);

    if (ctx->as_req.req_body.cname == NULL) {
	ret = init_as_req(context, ctx->flags, &ctx->cred,
			  ctx->addrs, ctx->etypes, &ctx->as_req);
	if (ret) {
	    free_init_creds_ctx(context, ctx);
	    return ret;
	}
    }

#define MAX_PA_COUNTER 10
    if (ctx->pa_counter > MAX_PA_COUNTER) {
	krb5_set_error_message(context, KRB5_GET_IN_TKT_LOOP,
			       N_("Looping %d times while getting "
				  "initial credentials", ""),
			       ctx->pa_counter);
	return KRB5_GET_IN_TKT_LOOP;
    }
    ctx->pa_counter++;

    _krb5_debug(context, 5, "krb5_get_init_creds: loop %d", ctx->pa_counter);

    /* Lets process the input packet */
    if (in && in->length) {
	krb5_kdc_rep rep;

	memset(&rep, 0, sizeof(rep));

	_krb5_debug(context, 5, "krb5_get_init_creds: processing input");

	ret = decode_AS_REP(in->data, in->length, &rep.kdc_rep, &size);
	if (ret == 0) {
	    unsigned eflags = EXTRACT_TICKET_AS_REQ | EXTRACT_TICKET_TIMESYNC;
	    krb5_data data;

	    /*
	     * Unwrap AS-REP
	     */
	    ASN1_MALLOC_ENCODE(Ticket, data.data, data.length,
			       &rep.kdc_rep.ticket, &size, ret);
	    if (ret)
		goto out;
	    heim_assert(data.length == size, "ASN.1 internal error");

	    ret = fast_unwrap_as_rep(context, ctx->nonce, &data,
				     &ctx->fast_state, &rep.kdc_rep);
	    krb5_data_free(&data);
	    if (ret)
		goto out;

	    /*
	     * Now check and extract the ticket
	     */

	    if (ctx->flags.canonicalize) {
		eflags |= EXTRACT_TICKET_ALLOW_SERVER_MISMATCH;
		eflags |= EXTRACT_TICKET_MATCH_REALM;
	    }
	    if (ctx->ic_flags & KRB5_INIT_CREDS_NO_C_CANON_CHECK)
		eflags |= EXTRACT_TICKET_ALLOW_CNAME_MISMATCH;

	    ret = process_pa_data_to_key(context, ctx, &ctx->cred,
					 &ctx->as_req, &rep.kdc_rep,
					 hostinfo, &ctx->fast_state.reply_key);
	    if (ret) {
		free_AS_REP(&rep.kdc_rep);
		goto out;
	    }

	    _krb5_debug(context, 5, "krb5_get_init_creds: extracting ticket");

	    ret = _krb5_extract_ticket(context,
				       &rep,
				       &ctx->cred,
				       ctx->fast_state.reply_key,
				       NULL,
				       KRB5_KU_AS_REP_ENC_PART,
				       NULL,
				       ctx->nonce,
				       eflags,
 				       &ctx->req_buffer,
 				       NULL,
 				       NULL);
	    if (ret == 0 && ctx->pk_init_ctx) {
		PA_DATA *pa_pkinit_kx;
		int idx = 0;

		pa_pkinit_kx =
		    krb5_find_padata(rep.kdc_rep.padata->val,
				     rep.kdc_rep.padata->len,
				     KRB5_PADATA_PKINIT_KX,
				     &idx);

		ret = _krb5_pk_kx_confirm(context, ctx->pk_init_ctx,
					  ctx->fast_state.reply_key,
					  &ctx->cred.session,
					  pa_pkinit_kx);
		if (ret)
		    krb5_set_error_message(context, ret,
					   N_("Failed to confirm PA-PKINIT-KX", ""));
		else if (pa_pkinit_kx != NULL)
		    ctx->ic_flags |= KRB5_INIT_CREDS_PKINIT_KX_VALID;
	    }
 	    if (ret == 0)
 		ret = copy_EncKDCRepPart(&rep.enc_part, &ctx->enc_part);
 
	    krb5_free_keyblock(context, ctx->fast_state.reply_key);
	    ctx->fast_state.reply_key = NULL;
	    *flags = 0;

	    free_AS_REP(&rep.kdc_rep);
	    free_EncASRepPart(&rep.enc_part);

	    return ret;

	} else {
	    /* let's try to parse it as a KRB-ERROR */

	    _krb5_debug(context, 5, "krb5_get_init_creds: got an error");

	    free_KRB_ERROR(&ctx->error);

	    ret = krb5_rd_error(context, in, &ctx->error);
	    if(ret && in->length && ((char*)in->data)[0] == 4)
		ret = KRB5KRB_AP_ERR_V4_REPLY;
	    if (ret) {
		_krb5_debug(context, 5, "krb5_get_init_creds: failed to read error");
		goto out;
	    }

	    /*
	     * Unwrap KRB-ERROR
	     */
	    ret = fast_unwrap_error(context, &ctx->fast_state, &ctx->error);
	    if (ret)
		goto out;

	    /*
	     *
	     */

	    ret = krb5_error_from_rd_error(context, &ctx->error, &ctx->cred);

	    _krb5_debug(context, 5, "krb5_get_init_creds: KRB-ERROR %d", ret);

	    /*
	     * If no preauth was set and KDC requires it, give it one
	     * more try.
	     */

	    if (ret == KRB5KDC_ERR_PREAUTH_REQUIRED) {

	        free_METHOD_DATA(&ctx->md);
	        memset(&ctx->md, 0, sizeof(ctx->md));

		if (ctx->error.e_data) {
		    ret = decode_METHOD_DATA(ctx->error.e_data->data,
					     ctx->error.e_data->length,
					     &ctx->md,
					     NULL);
		    if (ret)
			krb5_set_error_message(context, ret,
					       N_("Failed to decode METHOD-DATA", ""));
		} else {
		    krb5_set_error_message(context, ret,
					   N_("Preauth required but no preauth "
					      "options send by KDC", ""));
		}
	    } else if (ret == KRB5KRB_AP_ERR_SKEW && context->kdc_sec_offset == 0) {
		/*
		 * Try adapt to timeskrew when we are using pre-auth, and
		 * if there was a time skew, try again.
		 */
		krb5_set_real_time(context, ctx->error.stime, -1);
		if (context->kdc_sec_offset)
		    ret = 0;

		_krb5_debug(context, 10, "init_creds: err skew updateing kdc offset to %d",
			    context->kdc_sec_offset);

		ctx->used_pa_types = 0;

	    } else if (ret == KRB5_KDC_ERR_WRONG_REALM && ctx->flags.canonicalize) {
	        /* client referal to a new realm */

		if (ctx->error.crealm == NULL) {
		    krb5_set_error_message(context, ret,
					   N_("Got a client referral, not but no realm", ""));
		    goto out;
		}
		_krb5_debug(context, 5,
			    "krb5_get_init_creds: got referal to realm %s",
			    *ctx->error.crealm);

		ret = krb5_principal_set_realm(context,
					       ctx->cred.client,
					       *ctx->error.crealm);
		if (ret)
		    goto out;

		if (krb5_principal_is_krbtgt(context, ctx->cred.server)) {
		    ret = krb5_init_creds_set_service(context, ctx, NULL);
		    if (ret)
			goto out;
		}

		free_AS_REQ(&ctx->as_req);
		memset(&ctx->as_req, 0, sizeof(ctx->as_req));

		ctx->used_pa_types = 0;
	    } else if (ret == KRB5KDC_ERR_KEY_EXP && ctx->runflags.change_password == 0 && ctx->prompter) {
		char buf2[1024];

		ctx->runflags.change_password = 1;

		ctx->prompter(context, ctx->prompter_data, NULL, N_("Password has expired", ""), 0, NULL);


		/* try to avoid recursion */
		if (ctx->in_tkt_service != NULL && strcmp(ctx->in_tkt_service, "kadmin/changepw") == 0)
		    goto out;

                /* don't try to change password where then where none */
                if (ctx->prompter == NULL)
                    goto out;

		ret = change_password(context,
				      ctx->cred.client,
				      ctx->password,
				      buf2,
				      sizeof(buf2),
				      ctx->prompter,
				      ctx->prompter_data,
				      NULL);
		if (ret)
		    goto out;

		krb5_init_creds_set_password(context, ctx, buf2);

 		ctx->used_pa_types = 0;
		ret = 0;

 	    } else if (ret == KRB5KDC_ERR_PREAUTH_FAILED) {
 
 		if (ctx->fast_state.flags & KRB5_FAST_DISABLED)
 		    goto out;
 		if (ctx->fast_state.flags & (KRB5_FAST_REQUIRED | KRB5_FAST_EXPECTED))
 		    goto out;
 
 		_krb5_debug(context, 10, "preauth failed with FAST, "
			    "and told by KD or user, trying w/o FAST");
 
 		ctx->fast_state.flags |= KRB5_FAST_DISABLED;
 		ctx->used_pa_types = 0;
		ret = 0;
	    }
	    if (ret)
		goto out;
	}
    }

    if (ctx->as_req.req_body.cname == NULL) {
	ret = init_as_req(context, ctx->flags, &ctx->cred,
			  ctx->addrs, ctx->etypes, &ctx->as_req);
	if (ret) {
	    free_init_creds_ctx(context, ctx);
	    return ret;
	}
    }

    if (ctx->as_req.padata) {
	free_METHOD_DATA(ctx->as_req.padata);
	free(ctx->as_req.padata);
	ctx->as_req.padata = NULL;
    }

    /* Set a new nonce. */
    ctx->as_req.req_body.nonce = ctx->nonce;

    /* fill_in_md_data */
    ret = process_pa_data_to_md(context, &ctx->cred, &ctx->as_req, ctx,
				&ctx->md, &ctx->as_req.padata,
				ctx->prompter, ctx->prompter_data);
    if (ret)
	goto out;

    /*
     * Wrap with FAST
     */
    copy_AS_REQ(&ctx->as_req, &req2);

    ret = fast_wrap_req(context, &ctx->fast_state, &req2);
    if (ret) {
	free_AS_REQ(&req2);
	goto out;
    }

    krb5_data_free(&ctx->req_buffer);

    ASN1_MALLOC_ENCODE(AS_REQ,
		       ctx->req_buffer.data, ctx->req_buffer.length,
		       &req2, &len, ret);
    free_AS_REQ(&req2);
    if (ret)
	goto out;
    if(len != ctx->req_buffer.length)
	krb5_abortx(context, "internal error in ASN.1 encoder");

    out->data = ctx->req_buffer.data;
    out->length = ctx->req_buffer.length;

    *flags = KRB5_INIT_CREDS_STEP_FLAG_CONTINUE;

    return 0;
 out:
    return ret;
}
