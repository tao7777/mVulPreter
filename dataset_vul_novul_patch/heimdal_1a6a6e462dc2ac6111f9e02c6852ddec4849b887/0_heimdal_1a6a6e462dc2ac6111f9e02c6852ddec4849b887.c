_kdc_as_rep(kdc_request_t r,
	    krb5_data *reply,
	    const char *from,
	    struct sockaddr *from_addr,
	    int datagram_reply)
{
    krb5_context context = r->context;
    krb5_kdc_configuration *config = r->config;
    KDC_REQ *req = &r->req;
    KDC_REQ_BODY *b = NULL;
    AS_REP rep;
    KDCOptions f;
    krb5_enctype setype;
    krb5_error_code ret = 0;
    Key *skey;
    int found_pa = 0;
    int i, flags = HDB_F_FOR_AS_REQ;
    METHOD_DATA error_method;
    const PA_DATA *pa;

    memset(&rep, 0, sizeof(rep));
    error_method.len = 0;
    error_method.val = NULL;

    /*
     * Look for FAST armor and unwrap
     */
    ret = _kdc_fast_unwrap_request(r);
    if (ret) {
	_kdc_r_log(r, 0, "FAST unwrap request from %s failed: %d", from, ret);
	goto out;
    }

    b = &req->req_body;
    f = b->kdc_options;

    if (f.canonicalize)
	flags |= HDB_F_CANON;

    if(b->sname == NULL){
	ret = KRB5KRB_ERR_GENERIC;
	_kdc_set_e_text(r, "No server in request");
    } else{
	ret = _krb5_principalname2krb5_principal (context,
						  &r->server_princ,
						  *(b->sname),
						  b->realm);
	if (ret == 0)
	    ret = krb5_unparse_name(context, r->server_princ, &r->server_name);
    }
    if (ret) {
	kdc_log(context, config, 0,
		"AS-REQ malformed server name from %s", from);
	goto out;
    }
    if(b->cname == NULL){
	ret = KRB5KRB_ERR_GENERIC;
	_kdc_set_e_text(r, "No client in request");
    } else {
	ret = _krb5_principalname2krb5_principal (context,
						  &r->client_princ,
						  *(b->cname),
						  b->realm);
	if (ret)
	    goto out;

	ret = krb5_unparse_name(context, r->client_princ, &r->client_name);
    }
    if (ret) {
	kdc_log(context, config, 0,
		"AS-REQ malformed client name from %s", from);
	goto out;
    }

    kdc_log(context, config, 0, "AS-REQ %s from %s for %s",
	    r->client_name, from, r->server_name);

    /*
     *
     */

    if (_kdc_is_anonymous(context, r->client_princ)) {
	if (!_kdc_is_anon_request(b)) {
	    kdc_log(context, config, 0, "Anonymous ticket w/o anonymous flag");
	    ret = KRB5KDC_ERR_C_PRINCIPAL_UNKNOWN;
	    goto out;
	}
    } else if (_kdc_is_anon_request(b)) {
	kdc_log(context, config, 0,
		"Request for a anonymous ticket with non "
		"anonymous client name: %s", r->client_name);
	ret = KRB5KDC_ERR_C_PRINCIPAL_UNKNOWN;
	goto out;
    }

    /*
     *
     */

    ret = _kdc_db_fetch(context, config, r->client_princ,
			HDB_F_GET_CLIENT | flags, NULL,
			&r->clientdb, &r->client);
    if(ret == HDB_ERR_NOT_FOUND_HERE) {
	kdc_log(context, config, 5, "client %s does not have secrets at this KDC, need to proxy",
		r->client_name);
	goto out;
    } else if (ret == HDB_ERR_WRONG_REALM) {
	char *fixed_client_name = NULL;

	ret = krb5_unparse_name(context, r->client->entry.principal,
				&fixed_client_name);
	if (ret) {
	    goto out;
	}

	kdc_log(context, config, 0, "WRONG_REALM - %s -> %s",
		r->client_name, fixed_client_name);
	free(fixed_client_name);

	ret = _kdc_fast_mk_error(context, r,
				 &error_method,
				 r->armor_crypto,
				 &req->req_body,
				 KRB5_KDC_ERR_WRONG_REALM,
				 NULL,
				 r->server_princ,
				 NULL,
				 &r->client->entry.principal->realm,
				 NULL, NULL,
				 reply);
	goto out;
    } else if(ret){
	const char *msg = krb5_get_error_message(context, ret);
	kdc_log(context, config, 0, "UNKNOWN -- %s: %s", r->client_name, msg);
	krb5_free_error_message(context, msg);
	ret = KRB5KDC_ERR_C_PRINCIPAL_UNKNOWN;
	goto out;
    }
    ret = _kdc_db_fetch(context, config, r->server_princ,
			HDB_F_GET_SERVER|HDB_F_GET_KRBTGT | flags,
			NULL, NULL, &r->server);
    if(ret == HDB_ERR_NOT_FOUND_HERE) {
	kdc_log(context, config, 5, "target %s does not have secrets at this KDC, need to proxy",
		r->server_name);
	goto out;
    } else if(ret){
	const char *msg = krb5_get_error_message(context, ret);
	kdc_log(context, config, 0, "UNKNOWN -- %s: %s", r->server_name, msg);
	krb5_free_error_message(context, msg);
	ret = KRB5KDC_ERR_S_PRINCIPAL_UNKNOWN;
	goto out;
    }

    /*
     * Select a session enctype from the list of the crypto system
     * supported enctypes that is supported by the client and is one of
     * the enctype of the enctype of the service (likely krbtgt).
     *
     * The latter is used as a hint of what enctypes all KDC support,
     * to make sure a newer version of KDC won't generate a session
     * enctype that an older version of a KDC in the same realm can't
     * decrypt.
     */

    ret = _kdc_find_etype(context,
			  krb5_principal_is_krbtgt(context, r->server_princ) ?
			  config->tgt_use_strongest_session_key :
			  config->svc_use_strongest_session_key, FALSE,
			  r->client, b->etype.val, b->etype.len, &r->sessionetype,
			  NULL);
    if (ret) {
	kdc_log(context, config, 0,
		"Client (%s) from %s has no common enctypes with KDC "
		"to use for the session key",
		r->client_name, from);
	goto out;
    }

    /*
     * Pre-auth processing
     */

    if(req->padata){
	unsigned int n;

	log_patypes(context, config, req->padata);

	/* Check if preauth matching */

	for (n = 0; !found_pa && n < sizeof(pat) / sizeof(pat[0]); n++) {
	    if (pat[n].validate == NULL)
		continue;
	    if (r->armor_crypto == NULL && (pat[n].flags & PA_REQ_FAST))
		continue;

	    kdc_log(context, config, 5,
		    "Looking for %s pa-data -- %s", pat[n].name, r->client_name);
	    i = 0;
	    pa = _kdc_find_padata(req, &i, pat[n].type);
	    if (pa) {
		ret = pat[n].validate(r, pa);
		if (ret != 0) {
		    goto out;
		}
		kdc_log(context, config, 0,
			"%s pre-authentication succeeded -- %s",
			pat[n].name, r->client_name);
		found_pa = 1;
		r->et.flags.pre_authent = 1;
	    }
	}
    }

    if (found_pa == 0) {
	Key *ckey = NULL;
	size_t n;

	for (n = 0; n < sizeof(pat) / sizeof(pat[0]); n++) {
	    if ((pat[n].flags & PA_ANNOUNCE) == 0)
		continue;
	    ret = krb5_padata_add(context, &error_method,
				  pat[n].type, NULL, 0);
	    if (ret)
		goto out;
	}

	/*
	 * If there is a client key, send ETYPE_INFO{,2}
	 */
	ret = _kdc_find_etype(context,
			      config->preauth_use_strongest_session_key, TRUE,
			      r->client, b->etype.val, b->etype.len, NULL, &ckey);
	if (ret == 0) {

	    /*
	     * RFC4120 requires:
	     * - If the client only knows about old enctypes, then send
	     *   both info replies (we send 'info' first in the list).
	     * - If the client is 'modern', because it knows about 'new'
	     *   enctype types, then only send the 'info2' reply.
	     *
	     * Before we send the full list of etype-info data, we pick
	     * the client key we would have used anyway below, just pick
	     * that instead.
	     */

	    if (older_enctype(ckey->key.keytype)) {
		ret = get_pa_etype_info(context, config,
					&error_method, ckey);
		if (ret)
		    goto out;
	    }
	    ret = get_pa_etype_info2(context, config,
				     &error_method, ckey);
	    if (ret)
		goto out;
	}

	/* 
	 * send requre preauth is its required or anon is requested,
	 * anon is today only allowed via preauth mechanisms.
	 */
	if (require_preauth_p(r) || _kdc_is_anon_request(b)) {
	    ret = KRB5KDC_ERR_PREAUTH_REQUIRED;
	    _kdc_set_e_text(r, "Need to use PA-ENC-TIMESTAMP/PA-PK-AS-REQ");
	    goto out;
	}

	if (ckey == NULL) {
	    ret = KRB5KDC_ERR_CLIENT_NOTYET;
	    _kdc_set_e_text(r, "Doesn't have a client key available");
	    goto out;
	}
	krb5_free_keyblock_contents(r->context,  &r->reply_key);
	ret = krb5_copy_keyblock_contents(r->context, &ckey->key, &r->reply_key);
	if (ret)
	    goto out;
    }

    if (r->clientdb->hdb_auth_status) {
	r->clientdb->hdb_auth_status(context, r->clientdb, r->client, 
				     HDB_AUTH_SUCCESS);
    }

    /*
     * Verify flags after the user been required to prove its identity
     * with in a preauth mech.
     */

    ret = _kdc_check_access(context, config, r->client, r->client_name,
			    r->server, r->server_name,
			    req, &error_method);
    if(ret)
	goto out;

    /*
     * Select the best encryption type for the KDC with out regard to
     * the client since the client never needs to read that data.
     */

    ret = _kdc_get_preferred_key(context, config,
				 r->server, r->server_name,
				 &setype, &skey);
    if(ret)
	goto out;

    if(f.renew || f.validate || f.proxy || f.forwarded || f.enc_tkt_in_skey
       || (_kdc_is_anon_request(b) && !config->allow_anonymous)) {
	ret = KRB5KDC_ERR_BADOPTION;
	_kdc_set_e_text(r, "Bad KDC options");
	goto out;
    }

    /*
     * Build reply
     */

    rep.pvno = 5;
    rep.msg_type = krb_as_rep;

    if (_kdc_is_anonymous(context, r->client_princ)) {
	Realm anon_realm=KRB5_ANON_REALM;
	ret = copy_Realm(&anon_realm, &rep.crealm);
    } else
	ret = copy_Realm(&r->client->entry.principal->realm, &rep.crealm);
    if (ret)
	goto out;
    ret = _krb5_principal2principalname(&rep.cname, r->client->entry.principal);
    if (ret)
	goto out;

    rep.ticket.tkt_vno = 5;
    ret = copy_Realm(&r->server->entry.principal->realm, &rep.ticket.realm);
    if (ret)
	goto out;
    _krb5_principal2principalname(&rep.ticket.sname,
				  r->server->entry.principal);
    /* java 1.6 expects the name to be the same type, lets allow that
     * uncomplicated name-types. */
#define CNT(sp,t) (((sp)->sname->name_type) == KRB5_NT_##t)
    if (CNT(b, UNKNOWN) || CNT(b, PRINCIPAL) || CNT(b, SRV_INST) || CNT(b, SRV_HST) || CNT(b, SRV_XHST))
	rep.ticket.sname.name_type = b->sname->name_type;
#undef CNT

    r->et.flags.initial = 1;
    if(r->client->entry.flags.forwardable && r->server->entry.flags.forwardable)
	r->et.flags.forwardable = f.forwardable;
    else if (f.forwardable) {
	_kdc_set_e_text(r, "Ticket may not be forwardable");
	ret = KRB5KDC_ERR_POLICY;
	goto out;
    }
    if(r->client->entry.flags.proxiable && r->server->entry.flags.proxiable)
	r->et.flags.proxiable = f.proxiable;
    else if (f.proxiable) {
	_kdc_set_e_text(r, "Ticket may not be proxiable");
	ret = KRB5KDC_ERR_POLICY;
	goto out;
    }
    if(r->client->entry.flags.postdate && r->server->entry.flags.postdate)
	r->et.flags.may_postdate = f.allow_postdate;
    else if (f.allow_postdate){
	_kdc_set_e_text(r, "Ticket may not be postdate");
	ret = KRB5KDC_ERR_POLICY;
	goto out;
    }

    /* check for valid set of addresses */
    if(!_kdc_check_addresses(context, config, b->addresses, from_addr)) {
	_kdc_set_e_text(r, "Bad address list in requested");
	ret = KRB5KRB_AP_ERR_BADADDR;
	goto out;
    }

    ret = copy_PrincipalName(&rep.cname, &r->et.cname);
    if (ret)
	goto out;
    ret = copy_Realm(&rep.crealm, &r->et.crealm);
    if (ret)
	goto out;

    {
	time_t start;
	time_t t;
	
	start = r->et.authtime = kdc_time;

	if(f.postdated && req->req_body.from){
	    ALLOC(r->et.starttime);
	    start = *r->et.starttime = *req->req_body.from;
	    r->et.flags.invalid = 1;
	    r->et.flags.postdated = 1; /* XXX ??? */
	}
	_kdc_fix_time(&b->till);
	t = *b->till;

	/* be careful not overflowing */

	if(r->client->entry.max_life)
	    t = start + min(t - start, *r->client->entry.max_life);
	if(r->server->entry.max_life)
	    t = start + min(t - start, *r->server->entry.max_life);
#if 0
	t = min(t, start + realm->max_life);
#endif
	r->et.endtime = t;
	if(f.renewable_ok && r->et.endtime < *b->till){
	    f.renewable = 1;
	    if(b->rtime == NULL){
		ALLOC(b->rtime);
		*b->rtime = 0;
	    }
	    if(*b->rtime < *b->till)
		*b->rtime = *b->till;
	}
	if(f.renewable && b->rtime){
	    t = *b->rtime;
	    if(t == 0)
		t = MAX_TIME;
	    if(r->client->entry.max_renew)
		t = start + min(t - start, *r->client->entry.max_renew);
	    if(r->server->entry.max_renew)
		t = start + min(t - start, *r->server->entry.max_renew);
#if 0
	    t = min(t, start + realm->max_renew);
#endif
	    ALLOC(r->et.renew_till);
	    *r->et.renew_till = t;
	    r->et.flags.renewable = 1;
	}
    }

    if (_kdc_is_anon_request(b))
	r->et.flags.anonymous = 1;

    if(b->addresses){
	ALLOC(r->et.caddr);
	copy_HostAddresses(b->addresses, r->et.caddr);
    }

    r->et.transited.tr_type = DOMAIN_X500_COMPRESS;
    krb5_data_zero(&r->et.transited.contents);

    /* The MIT ASN.1 library (obviously) doesn't tell lengths encoded
     * as 0 and as 0x80 (meaning indefinite length) apart, and is thus
     * incapable of correctly decoding SEQUENCE OF's of zero length.
     *
     * To fix this, always send at least one no-op last_req
     *
     * If there's a pw_end or valid_end we will use that,
     * otherwise just a dummy lr.
     */
    r->ek.last_req.val = malloc(2 * sizeof(*r->ek.last_req.val));
    if (r->ek.last_req.val == NULL) {
	ret = ENOMEM;
	goto out;
    }
    r->ek.last_req.len = 0;
    if (r->client->entry.pw_end
	&& (config->kdc_warn_pwexpire == 0
	    || kdc_time + config->kdc_warn_pwexpire >= *r->client->entry.pw_end)) {
	r->ek.last_req.val[r->ek.last_req.len].lr_type  = LR_PW_EXPTIME;
	r->ek.last_req.val[r->ek.last_req.len].lr_value = *r->client->entry.pw_end;
	++r->ek.last_req.len;
    }
    if (r->client->entry.valid_end) {
	r->ek.last_req.val[r->ek.last_req.len].lr_type  = LR_ACCT_EXPTIME;
	r->ek.last_req.val[r->ek.last_req.len].lr_value = *r->client->entry.valid_end;
	++r->ek.last_req.len;
    }
    if (r->ek.last_req.len == 0) {
	r->ek.last_req.val[r->ek.last_req.len].lr_type  = LR_NONE;
	r->ek.last_req.val[r->ek.last_req.len].lr_value = 0;
	++r->ek.last_req.len;
    }
    r->ek.nonce = b->nonce;
    if (r->client->entry.valid_end || r->client->entry.pw_end) {
	ALLOC(r->ek.key_expiration);
	if (r->client->entry.valid_end) {
	    if (r->client->entry.pw_end)
		*r->ek.key_expiration = min(*r->client->entry.valid_end,
					 *r->client->entry.pw_end);
	    else
		*r->ek.key_expiration = *r->client->entry.valid_end;
	} else
	    *r->ek.key_expiration = *r->client->entry.pw_end;
    } else
	r->ek.key_expiration = NULL;
    r->ek.flags = r->et.flags;
    r->ek.authtime = r->et.authtime;
    if (r->et.starttime) {
	ALLOC(r->ek.starttime);
	*r->ek.starttime = *r->et.starttime;
    }
    r->ek.endtime = r->et.endtime;
    if (r->et.renew_till) {
	ALLOC(r->ek.renew_till);
	*r->ek.renew_till = *r->et.renew_till;
    }
    ret = copy_Realm(&rep.ticket.realm, &r->ek.srealm);
    if (ret)
	goto out;
    ret = copy_PrincipalName(&rep.ticket.sname, &r->ek.sname);
    if (ret)
	goto out;
    if(r->et.caddr){
	ALLOC(r->ek.caddr);
	copy_HostAddresses(r->et.caddr, r->ek.caddr);
    }

    /*
     * Check and session and reply keys
     */

    if (r->session_key.keytype == ETYPE_NULL) {
	ret = krb5_generate_random_keyblock(context, r->sessionetype, &r->session_key);
	if (ret)
	    goto out;
    }

    if (r->reply_key.keytype == ETYPE_NULL) {
	_kdc_set_e_text(r, "Client have no reply key");
	ret = KRB5KDC_ERR_CLIENT_NOTYET;
	goto out;
    }

    ret = copy_EncryptionKey(&r->session_key, &r->et.key);
    if (ret)
	goto out;

    ret = copy_EncryptionKey(&r->session_key, &r->ek.key);
    if (ret)
	goto out;

    if (r->outpadata.len) {

	ALLOC(rep.padata);
	if (rep.padata == NULL) {
	    ret = ENOMEM;
	    goto out;
	}
	ret = copy_METHOD_DATA(&r->outpadata, rep.padata);
	if (ret)
	    goto out;
    }

    /* Add the PAC */
    if (send_pac_p(context, req)) {
	generate_pac(r, skey);
    }

    _kdc_log_timestamp(context, config, "AS-REQ", r->et.authtime, r->et.starttime,
		       r->et.endtime, r->et.renew_till);

    /* do this as the last thing since this signs the EncTicketPart */
    ret = _kdc_add_KRB5SignedPath(context,
				  config,
				  r->server,
				  setype,
				  r->client->entry.principal,
				  NULL,
				  NULL,
				  &r->et);
    if (ret)
	goto out;

    log_as_req(context, config, r->reply_key.keytype, setype, b);

    /*
     * We always say we support FAST/enc-pa-rep
     */

    r->et.flags.enc_pa_rep = r->ek.flags.enc_pa_rep = 1;

    /*
     * Add REQ_ENC_PA_REP if client supports it
     */

    i = 0;
    pa = _kdc_find_padata(req, &i, KRB5_PADATA_REQ_ENC_PA_REP);
    if (pa) {

	ret = add_enc_pa_rep(r);
	if (ret) {
	    const char *msg = krb5_get_error_message(r->context, ret);
	    _kdc_r_log(r, 0, "add_enc_pa_rep failed: %s: %d", msg, ret);
	    krb5_free_error_message(r->context, msg);
	    goto out;
	}
    }

    /*
     *
     */

    ret = _kdc_encode_reply(context, config,
			    r->armor_crypto, req->req_body.nonce,
			    &rep, &r->et, &r->ek, setype, r->server->entry.kvno,
			    &skey->key, r->client->entry.kvno,
			    &r->reply_key, 0, &r->e_text, reply);
    if (ret)
	goto out;

    /*
     * Check if message too large
     */
    if (datagram_reply && reply->length > config->max_datagram_reply_length) {
	krb5_data_free(reply);
	ret = KRB5KRB_ERR_RESPONSE_TOO_BIG;
	_kdc_set_e_text(r, "Reply packet too large");
    }

out:
    free_AS_REP(&rep);

     /*
      * In case of a non proxy error, build an error message.
      */
    if (ret != 0 && ret != HDB_ERR_NOT_FOUND_HERE && reply->length == 0) {
 	ret = _kdc_fast_mk_error(context, r,
 				 &error_method,
 				 r->armor_crypto,
 				 &req->req_body,
 				 ret, r->e_text,
 				 r->server_princ,
				 r->client_princ ?
                                     &r->client_princ->name : NULL,
				 r->client_princ ?
                                     &r->client_princ->realm : NULL,
 				 NULL, NULL,
 				 reply);
 	if (ret)
	    goto out2;
    }
out2:
    free_EncTicketPart(&r->et);
    free_EncKDCRepPart(&r->ek);
    free_KDCFastState(&r->fast);

    if (error_method.len)
	free_METHOD_DATA(&error_method);
    if (r->outpadata.len)
	free_METHOD_DATA(&r->outpadata);
    if (r->client_princ) {
	krb5_free_principal(context, r->client_princ);
	r->client_princ = NULL;
    }
    if (r->client_name) {
	free(r->client_name);
	r->client_name = NULL;
    }
    if (r->server_princ){
	krb5_free_principal(context, r->server_princ);
	r->server_princ = NULL;
    }
    if (r->server_name) {
	free(r->server_name);
	r->server_name = NULL;
    }
    if (r->client)
	_kdc_free_ent(context, r->client);
    if (r->server)
	_kdc_free_ent(context, r->server);
    if (r->armor_crypto) {
	krb5_crypto_destroy(r->context, r->armor_crypto);
	r->armor_crypto = NULL;
    }
    krb5_free_keyblock_contents(r->context, &r->reply_key);
    krb5_free_keyblock_contents(r->context, &r->session_key);
    return ret;
}
