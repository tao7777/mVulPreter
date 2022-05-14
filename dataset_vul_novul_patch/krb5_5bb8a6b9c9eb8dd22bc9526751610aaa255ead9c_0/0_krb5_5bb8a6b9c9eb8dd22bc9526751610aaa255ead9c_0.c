svcauth_gss_accept_sec_context(struct svc_req *rqst,
			       struct rpc_gss_init_res *gr)
{
	struct svc_rpc_gss_data	*gd;
	struct rpc_gss_cred	*gc;
	gss_buffer_desc		 recv_tok, seqbuf;
	gss_OID			 mech;
	OM_uint32		 maj_stat = 0, min_stat = 0, ret_flags, seq;

	log_debug("in svcauth_gss_accept_context()");

	gd = SVCAUTH_PRIVATE(rqst->rq_xprt->xp_auth);
	gc = (struct rpc_gss_cred *)rqst->rq_clntcred;
	memset(gr, 0, sizeof(*gr));

	/* Deserialize arguments. */
	memset(&recv_tok, 0, sizeof(recv_tok));

	if (!svc_getargs(rqst->rq_xprt, xdr_rpc_gss_init_args,
			 (caddr_t)&recv_tok))
		return (FALSE);

	gr->gr_major = gss_accept_sec_context(&gr->gr_minor,
					      &gd->ctx,
					      svcauth_gss_creds,
					      &recv_tok,
					      GSS_C_NO_CHANNEL_BINDINGS,
					      &gd->client_name,
					      &mech,
					      &gr->gr_token,
					      &ret_flags,
					      NULL,
					      NULL);

	svc_freeargs(rqst->rq_xprt, xdr_rpc_gss_init_args, (caddr_t)&recv_tok);

	log_status("accept_sec_context", gr->gr_major, gr->gr_minor);
	if (gr->gr_major != GSS_S_COMPLETE &&
	    gr->gr_major != GSS_S_CONTINUE_NEEDED) {
		badauth(gr->gr_major, gr->gr_minor, rqst->rq_xprt);
 		gd->ctx = GSS_C_NO_CONTEXT;
 		goto errout;
 	}
	gr->gr_ctx.value = "xxxx";
	gr->gr_ctx.length = 4;
 
 	/* gr->gr_win = 0x00000005; ANDROS: for debugging linux kernel version...  */
 	gr->gr_win = sizeof(gd->seqmask) * 8;

	/* Save client info. */
	gd->sec.mech = mech;
	gd->sec.qop = GSS_C_QOP_DEFAULT;
	gd->sec.svc = gc->gc_svc;
	gd->seq = gc->gc_seq;
	gd->win = gr->gr_win;

	if (gr->gr_major == GSS_S_COMPLETE) {
#ifdef SPKM
		/* spkm3: no src_name (anonymous) */
		if(!g_OID_equal(gss_mech_spkm3, mech)) {
#endif
		    maj_stat = gss_display_name(&min_stat, gd->client_name,
					    &gd->cname, &gd->sec.mech);
#ifdef SPKM
		}
#endif
		if (maj_stat != GSS_S_COMPLETE) {
			log_status("display_name", maj_stat, min_stat);
			goto errout;
		}
#ifdef DEBUG
#ifdef HAVE_HEIMDAL
		log_debug("accepted context for %.*s with "
			  "<mech {}, qop %d, svc %d>",
			  gd->cname.length, (char *)gd->cname.value,
			  gd->sec.qop, gd->sec.svc);
#else
		{
			gss_buffer_desc mechname;

			gss_oid_to_str(&min_stat, mech, &mechname);

			log_debug("accepted context for %.*s with "
				  "<mech %.*s, qop %d, svc %d>",
				  gd->cname.length, (char *)gd->cname.value,
				  mechname.length, (char *)mechname.value,
				  gd->sec.qop, gd->sec.svc);

			gss_release_buffer(&min_stat, &mechname);
		}
#endif
#endif /* DEBUG */
		seq = htonl(gr->gr_win);
		seqbuf.value = &seq;
		seqbuf.length = sizeof(seq);

		gss_release_buffer(&min_stat, &gd->checksum);
		maj_stat = gss_sign(&min_stat, gd->ctx, GSS_C_QOP_DEFAULT,
				    &seqbuf, &gd->checksum);

		if (maj_stat != GSS_S_COMPLETE) {
			goto errout;
		}


		rqst->rq_xprt->xp_verf.oa_flavor = RPCSEC_GSS;
		rqst->rq_xprt->xp_verf.oa_base = gd->checksum.value;
		rqst->rq_xprt->xp_verf.oa_length = gd->checksum.length;
	}
	return (TRUE);
errout:
	gss_release_buffer(&min_stat, &gr->gr_token);
	return (FALSE);
}
