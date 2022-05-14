tgs_make_reply(krb5_context context,
	       krb5_kdc_configuration *config,
	       KDC_REQ_BODY *b,
	       krb5_const_principal tgt_name,
	       const EncTicketPart *tgt,
	       const krb5_keyblock *replykey,
	       int rk_is_subkey,
	       const EncryptionKey *serverkey,
	       const krb5_keyblock *sessionkey,
	       krb5_kvno kvno,
	       AuthorizationData *auth_data,
	       hdb_entry_ex *server,
	       krb5_principal server_principal,
 	       const char *server_name,
 	       hdb_entry_ex *client,
 	       krb5_principal client_principal,
 	       hdb_entry_ex *krbtgt,
 	       krb5_enctype krbtgt_etype,
 	       krb5_principals spp,
	       const krb5_data *rspac,
	       const METHOD_DATA *enc_pa_data,
	       const char **e_text,
	       krb5_data *reply)
{
    KDC_REP rep;
    EncKDCRepPart ek;
    EncTicketPart et;
    KDCOptions f = b->kdc_options;
    krb5_error_code ret;
    int is_weak = 0;

    memset(&rep, 0, sizeof(rep));
    memset(&et, 0, sizeof(et));
    memset(&ek, 0, sizeof(ek));

    rep.pvno = 5;
    rep.msg_type = krb_tgs_rep;

    et.authtime = tgt->authtime;
    _kdc_fix_time(&b->till);
    et.endtime = min(tgt->endtime, *b->till);
    ALLOC(et.starttime);
    *et.starttime = kdc_time;

    ret = check_tgs_flags(context, config, b, tgt, &et);
    if(ret)
	goto out;

    /* We should check the transited encoding if:
       1) the request doesn't ask not to be checked
       2) globally enforcing a check
       3) principal requires checking
       4) we allow non-check per-principal, but principal isn't marked as allowing this
       5) we don't globally allow this
    */

#define GLOBAL_FORCE_TRANSITED_CHECK		\
    (config->trpolicy == TRPOLICY_ALWAYS_CHECK)
#define GLOBAL_ALLOW_PER_PRINCIPAL			\
    (config->trpolicy == TRPOLICY_ALLOW_PER_PRINCIPAL)
#define GLOBAL_ALLOW_DISABLE_TRANSITED_CHECK			\
    (config->trpolicy == TRPOLICY_ALWAYS_HONOUR_REQUEST)

/* these will consult the database in future release */
#define PRINCIPAL_FORCE_TRANSITED_CHECK(P)		0
#define PRINCIPAL_ALLOW_DISABLE_TRANSITED_CHECK(P)	0

    ret = fix_transited_encoding(context, config,
				 !f.disable_transited_check ||
				 GLOBAL_FORCE_TRANSITED_CHECK ||
				 PRINCIPAL_FORCE_TRANSITED_CHECK(server) ||
				 !((GLOBAL_ALLOW_PER_PRINCIPAL &&
				    PRINCIPAL_ALLOW_DISABLE_TRANSITED_CHECK(server)) ||
				   GLOBAL_ALLOW_DISABLE_TRANSITED_CHECK),
 				 &tgt->transited, &et,
 				 krb5_principal_get_realm(context, client_principal),
 				 krb5_principal_get_realm(context, server->entry.principal),
				 krb5_principal_get_realm(context, krbtgt->entry.principal));
     if(ret)
 	goto out;
 
    copy_Realm(&server_principal->realm, &rep.ticket.realm);
    _krb5_principal2principalname(&rep.ticket.sname, server_principal);
    copy_Realm(&tgt_name->realm, &rep.crealm);
/*
    if (f.request_anonymous)
	_kdc_make_anonymous_principalname (&rep.cname);
    else */

    copy_PrincipalName(&tgt_name->name, &rep.cname);
    rep.ticket.tkt_vno = 5;

    ek.caddr = et.caddr;

    {
	time_t life;
	life = et.endtime - *et.starttime;
	if(client && client->entry.max_life)
	    life = min(life, *client->entry.max_life);
	if(server->entry.max_life)
	    life = min(life, *server->entry.max_life);
	et.endtime = *et.starttime + life;
    }
    if(f.renewable_ok && tgt->flags.renewable &&
       et.renew_till == NULL && et.endtime < *b->till &&
       tgt->renew_till != NULL)
    {
	et.flags.renewable = 1;
	ALLOC(et.renew_till);
	*et.renew_till = *b->till;
    }
    if(et.renew_till){
	time_t renew;
	renew = *et.renew_till - *et.starttime;
	if(client && client->entry.max_renew)
	    renew = min(renew, *client->entry.max_renew);
	if(server->entry.max_renew)
	    renew = min(renew, *server->entry.max_renew);
	*et.renew_till = *et.starttime + renew;
    }

    if(et.renew_till){
	*et.renew_till = min(*et.renew_till, *tgt->renew_till);
	*et.starttime = min(*et.starttime, *et.renew_till);
	et.endtime = min(et.endtime, *et.renew_till);
    }

    *et.starttime = min(*et.starttime, et.endtime);

    if(*et.starttime == et.endtime){
	ret = KRB5KDC_ERR_NEVER_VALID;
	goto out;
    }
    if(et.renew_till && et.endtime == *et.renew_till){
	free(et.renew_till);
	et.renew_till = NULL;
	et.flags.renewable = 0;
    }

    et.flags.pre_authent = tgt->flags.pre_authent;
    et.flags.hw_authent  = tgt->flags.hw_authent;
    et.flags.anonymous   = tgt->flags.anonymous;
    et.flags.ok_as_delegate = server->entry.flags.ok_as_delegate;

    if(rspac->length) {
	/*
	 * No not need to filter out the any PAC from the
	 * auth_data since it's signed by the KDC.
	 */
	ret = _kdc_tkt_add_if_relevant_ad(context, &et,
					  KRB5_AUTHDATA_WIN2K_PAC, rspac);
	if (ret)
	    goto out;
    }

    if (auth_data) {
	unsigned int i = 0;

	/* XXX check authdata */

	if (et.authorization_data == NULL) {
	    et.authorization_data = calloc(1, sizeof(*et.authorization_data));
	    if (et.authorization_data == NULL) {
		ret = ENOMEM;
		krb5_set_error_message(context, ret, "malloc: out of memory");
		goto out;
	    }
	}
	for(i = 0; i < auth_data->len ; i++) {
	    ret = add_AuthorizationData(et.authorization_data, &auth_data->val[i]);
	    if (ret) {
		krb5_set_error_message(context, ret, "malloc: out of memory");
		goto out;
	    }
	}

	/* Filter out type KRB5SignedPath */
	ret = find_KRB5SignedPath(context, et.authorization_data, NULL);
	if (ret == 0) {
	    if (et.authorization_data->len == 1) {
		free_AuthorizationData(et.authorization_data);
		free(et.authorization_data);
		et.authorization_data = NULL;
	    } else {
		AuthorizationData *ad = et.authorization_data;
		free_AuthorizationDataElement(&ad->val[ad->len - 1]);
		ad->len--;
	    }
	}
    }

    ret = krb5_copy_keyblock_contents(context, sessionkey, &et.key);
    if (ret)
	goto out;
    et.crealm = tgt_name->realm;
    et.cname = tgt_name->name;

    ek.key = et.key;
    /* MIT must have at least one last_req */
    ek.last_req.val = calloc(1, sizeof(*ek.last_req.val));
    if (ek.last_req.val == NULL) {
	ret = ENOMEM;
	goto out;
    }
    ek.last_req.len = 1; /* set after alloc to avoid null deref on cleanup */
    ek.nonce = b->nonce;
    ek.flags = et.flags;
    ek.authtime = et.authtime;
    ek.starttime = et.starttime;
    ek.endtime = et.endtime;
    ek.renew_till = et.renew_till;
    ek.srealm = rep.ticket.realm;
    ek.sname = rep.ticket.sname;

    _kdc_log_timestamp(context, config, "TGS-REQ", et.authtime, et.starttime,
		       et.endtime, et.renew_till);

    /* Don't sign cross realm tickets, they can't be checked anyway */
    {
	char *r = get_krbtgt_realm(&ek.sname);

	if (r == NULL || strcmp(r, ek.srealm) == 0) {
	    ret = _kdc_add_KRB5SignedPath(context,
					  config,
					  krbtgt,
					  krbtgt_etype,
					  client_principal,
					  NULL,
					  spp,
					  &et);
	    if (ret)
		goto out;
	}
    }

    if (enc_pa_data->len) {
	rep.padata = calloc(1, sizeof(*rep.padata));
	if (rep.padata == NULL) {
	    ret = ENOMEM;
	    goto out;
	}
	ret = copy_METHOD_DATA(enc_pa_data, rep.padata);
	if (ret)
	    goto out;
    }

    if (krb5_enctype_valid(context, serverkey->keytype) != 0
	&& _kdc_is_weak_exception(server->entry.principal, serverkey->keytype))
    {
	krb5_enctype_enable(context, serverkey->keytype);
	is_weak = 1;
    }


    /* It is somewhat unclear where the etype in the following
       encryption should come from. What we have is a session
       key in the passed tgt, and a list of preferred etypes
       *for the new ticket*. Should we pick the best possible
       etype, given the keytype in the tgt, or should we look
       at the etype list here as well?  What if the tgt
       session key is DES3 and we want a ticket with a (say)
       CAST session key. Should the DES3 etype be added to the
       etype list, even if we don't want a session key with
       DES3? */
    ret = _kdc_encode_reply(context, config, NULL, 0,
			    &rep, &et, &ek, serverkey->keytype,
			    kvno,
			    serverkey, 0, replykey, rk_is_subkey,
			    e_text, reply);
    if (is_weak)
	krb5_enctype_disable(context, serverkey->keytype);

out:
    free_TGS_REP(&rep);
    free_TransitedEncoding(&et.transited);
    if(et.starttime)
	free(et.starttime);
    if(et.renew_till)
	free(et.renew_till);
    if(et.authorization_data) {
	free_AuthorizationData(et.authorization_data);
	free(et.authorization_data);
    }
    free_LastReq(&ek.last_req);
    memset(et.key.keyvalue.data, 0, et.key.keyvalue.length);
    free_EncryptionKey(&et.key);
    return ret;
}
