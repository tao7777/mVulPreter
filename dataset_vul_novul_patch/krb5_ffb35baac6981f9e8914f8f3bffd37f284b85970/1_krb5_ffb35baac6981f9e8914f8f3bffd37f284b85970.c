finish_process_as_req(struct as_req_state *state, krb5_error_code errcode)
{
    krb5_key_data *server_key;
    krb5_keyblock *as_encrypting_key = NULL;
    krb5_data *response = NULL;
    const char *emsg = 0;
    int did_log = 0;
    loop_respond_fn oldrespond;
    void *oldarg;
    kdc_realm_t *kdc_active_realm = state->active_realm;
    krb5_audit_state *au_state = state->au_state;

    assert(state);
    oldrespond = state->respond;
    oldarg = state->arg;

    if (errcode)
        goto egress;

    au_state->stage = ENCR_REP;

    if ((errcode = validate_forwardable(state->request, *state->client,
                                        *state->server, state->kdc_time,
                                        &state->status))) {
        errcode += ERROR_TABLE_BASE_krb5;
        goto egress;
    }

    errcode = check_indicators(kdc_context, state->server,
                               state->auth_indicators);
    if (errcode) {
        state->status = "HIGHER_AUTHENTICATION_REQUIRED";
        goto egress;
    }

    state->ticket_reply.enc_part2 = &state->enc_tkt_reply;

    /*
     * Find the server key
     */
    if ((errcode = krb5_dbe_find_enctype(kdc_context, state->server,
                                         -1, /* ignore keytype   */
                                         -1, /* Ignore salttype  */
                                         0,  /* Get highest kvno */
                                         &server_key))) {
        state->status = "FINDING_SERVER_KEY";
        goto egress;
    }

    /*
     * Convert server->key into a real key
     * (it may be encrypted in the database)
     *
     *  server_keyblock is later used to generate auth data signatures
     */
    if ((errcode = krb5_dbe_decrypt_key_data(kdc_context, NULL,
                                             server_key,
                                             &state->server_keyblock,
                                             NULL))) {
        state->status = "DECRYPT_SERVER_KEY";
        goto egress;
    }

    /* Start assembling the response */
    state->reply.msg_type = KRB5_AS_REP;
    state->reply.client = state->enc_tkt_reply.client; /* post canonization */
    state->reply.ticket = &state->ticket_reply;
    state->reply_encpart.session = &state->session_key;
    if ((errcode = fetch_last_req_info(state->client,
                                       &state->reply_encpart.last_req))) {
        state->status = "FETCH_LAST_REQ";
        goto egress;
    }
    state->reply_encpart.nonce = state->request->nonce;
    state->reply_encpart.key_exp = get_key_exp(state->client);
    state->reply_encpart.flags = state->enc_tkt_reply.flags;
    state->reply_encpart.server = state->ticket_reply.server;

    /* copy the time fields EXCEPT for authtime; it's location
     *  is used for ktime
     */
    state->reply_encpart.times = state->enc_tkt_reply.times;
    state->reply_encpart.times.authtime = state->authtime = state->kdc_time;

    state->reply_encpart.caddrs = state->enc_tkt_reply.caddrs;
    state->reply_encpart.enc_padata = NULL;

    /* Fetch the padata info to be returned (do this before
     *  authdata to handle possible replacement of reply key
     */
    errcode = return_padata(kdc_context, &state->rock, state->req_pkt,
                            state->request, &state->reply,
                            &state->client_keyblock, &state->pa_context);
    if (errcode) {
        state->status = "KDC_RETURN_PADATA";
        goto egress;
    }

    /* If we didn't find a client long-term key and no preauth mechanism
     * replaced the reply key, error out now. */
    if (state->client_keyblock.enctype == ENCTYPE_NULL) {
        state->status = "CANT_FIND_CLIENT_KEY";
        errcode = KRB5KDC_ERR_ETYPE_NOSUPP;
        goto egress;
    }

    errcode = handle_authdata(kdc_context,
                              state->c_flags,
                              state->client,
                              state->server,
                              NULL,
                              state->local_tgt,
                              &state->client_keyblock,
                              &state->server_keyblock,
                              NULL,
                              state->req_pkt,
                              state->request,
                              NULL, /* for_user_princ */
                              NULL, /* enc_tkt_request */
                              state->auth_indicators,
                              &state->enc_tkt_reply);
    if (errcode) {
        krb5_klog_syslog(LOG_INFO, _("AS_REQ : handle_authdata (%d)"),
                         errcode);
        state->status = "HANDLE_AUTHDATA";
        goto egress;
    }

    errcode = krb5_encrypt_tkt_part(kdc_context, &state->server_keyblock,
                                    &state->ticket_reply);
    if (errcode) {
        state->status = "ENCRYPT_TICKET";
        goto egress;
    }

    errcode = kau_make_tkt_id(kdc_context, &state->ticket_reply,
                              &au_state->tkt_out_id);
    if (errcode) {
        state->status = "GENERATE_TICKET_ID";
        goto egress;
    }

    state->ticket_reply.enc_part.kvno = server_key->key_data_kvno;
    errcode = kdc_fast_response_handle_padata(state->rstate,
                                              state->request,
                                              &state->reply,
                                              state->client_keyblock.enctype);
    if (errcode) {
        state->status = "MAKE_FAST_RESPONSE";
        goto egress;
    }

    /* now encode/encrypt the response */

    state->reply.enc_part.enctype = state->client_keyblock.enctype;

    errcode = kdc_fast_handle_reply_key(state->rstate, &state->client_keyblock,
                                        &as_encrypting_key);
    if (errcode) {
        state->status = "MAKE_FAST_REPLY_KEY";
        goto egress;
    }
    errcode = return_enc_padata(kdc_context, state->req_pkt, state->request,
                                as_encrypting_key, state->server,
                                &state->reply_encpart, FALSE);
    if (errcode) {
        state->status = "KDC_RETURN_ENC_PADATA";
        goto egress;
    }

    if (kdc_fast_hide_client(state->rstate))
        state->reply.client = (krb5_principal)krb5_anonymous_principal();
    errcode = krb5_encode_kdc_rep(kdc_context, KRB5_AS_REP,
                                  &state->reply_encpart, 0,
                                  as_encrypting_key,
                                  &state->reply, &response);
    if (state->client_key != NULL)
        state->reply.enc_part.kvno = state->client_key->key_data_kvno;
    if (errcode) {
        state->status = "ENCODE_KDC_REP";
        goto egress;
    }

    /* these parts are left on as a courtesy from krb5_encode_kdc_rep so we
       can use them in raw form if needed.  But, we don't... */
    memset(state->reply.enc_part.ciphertext.data, 0,
           state->reply.enc_part.ciphertext.length);
    free(state->reply.enc_part.ciphertext.data);

    log_as_req(kdc_context, state->local_addr, state->remote_addr,
               state->request, &state->reply, state->client, state->cname,
               state->server, state->sname, state->authtime, 0, 0, 0);
     did_log = 1;
 
 egress:
    if (errcode != 0)
        assert (state->status != 0);
 
     au_state->status = state->status;
     au_state->reply = &state->reply;
    kau_as_req(kdc_context,
              (errcode || state->preauth_err) ? FALSE : TRUE, au_state);
    kau_free_kdc_req(au_state);

    free_padata_context(kdc_context, state->pa_context);
    if (as_encrypting_key)
        krb5_free_keyblock(kdc_context, as_encrypting_key);
    if (errcode)
        emsg = krb5_get_error_message(kdc_context, errcode);

    if (state->status) {
        log_as_req(kdc_context, state->local_addr, state->remote_addr,
                   state->request, &state->reply, state->client,
                   state->cname, state->server, state->sname, state->authtime,
                   state->status, errcode, emsg);
        did_log = 1;
    }
    if (errcode) {
        if (state->status == 0) {
            state->status = emsg;
        }
        if (errcode != KRB5KDC_ERR_DISCARD) {
            errcode -= ERROR_TABLE_BASE_krb5;
            if (errcode < 0 || errcode > KRB_ERR_MAX)
                errcode = KRB_ERR_GENERIC;

            errcode = prepare_error_as(state->rstate, state->request,
                                       state->local_tgt, errcode,
                                       state->e_data, state->typed_e_data,
                                       ((state->client != NULL) ?
                                        state->client->princ : NULL),
                                       &response, state->status);
            state->status = 0;
        }
    }

    if (emsg)
        krb5_free_error_message(kdc_context, emsg);
    if (state->enc_tkt_reply.authorization_data != NULL)
        krb5_free_authdata(kdc_context,
                           state->enc_tkt_reply.authorization_data);
    if (state->server_keyblock.contents != NULL)
        krb5_free_keyblock_contents(kdc_context, &state->server_keyblock);
    if (state->client_keyblock.contents != NULL)
        krb5_free_keyblock_contents(kdc_context, &state->client_keyblock);
    if (state->reply.padata != NULL)
        krb5_free_pa_data(kdc_context, state->reply.padata);
    if (state->reply_encpart.enc_padata)
        krb5_free_pa_data(kdc_context, state->reply_encpart.enc_padata);

    if (state->cname != NULL)
        free(state->cname);
    if (state->sname != NULL)
        free(state->sname);
    krb5_db_free_principal(kdc_context, state->client);
    krb5_db_free_principal(kdc_context, state->server);
    krb5_db_free_principal(kdc_context, state->local_tgt_storage);
    if (state->session_key.contents != NULL)
        krb5_free_keyblock_contents(kdc_context, &state->session_key);
    if (state->ticket_reply.enc_part.ciphertext.data != NULL) {
        memset(state->ticket_reply.enc_part.ciphertext.data , 0,
               state->ticket_reply.enc_part.ciphertext.length);
        free(state->ticket_reply.enc_part.ciphertext.data);
    }

    krb5_free_pa_data(kdc_context, state->e_data);
    krb5_free_data(kdc_context, state->inner_body);
    kdc_free_rstate(state->rstate);
    krb5_free_kdc_req(kdc_context, state->request);
    k5_free_data_ptr_list(state->auth_indicators);
    assert(did_log != 0);

    free(state);
    (*oldrespond)(oldarg, errcode, response);
}
