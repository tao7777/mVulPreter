gssrpc__svcauth_gss(struct svc_req *rqst, struct rpc_msg *msg,
	bool_t *no_dispatch)
{
	enum auth_stat		 retstat;
	XDR	 		 xdrs;
	SVCAUTH			*auth;
	struct svc_rpc_gss_data	*gd;
	struct rpc_gss_cred	*gc;
	struct rpc_gss_init_res	 gr;
	int			 call_stat, offset;
	OM_uint32		 min_stat;

	log_debug("in svcauth_gss()");

	/* Initialize reply. */
	rqst->rq_xprt->xp_verf = gssrpc__null_auth;

	/* Allocate and set up server auth handle. */
	if (rqst->rq_xprt->xp_auth == NULL ||
	    rqst->rq_xprt->xp_auth == &svc_auth_none) {
		if ((auth = calloc(sizeof(*auth), 1)) == NULL) {
			fprintf(stderr, "svcauth_gss: out_of_memory\n");
			return (AUTH_FAILED);
		}
		if ((gd = calloc(sizeof(*gd), 1)) == NULL) {
			fprintf(stderr, "svcauth_gss: out_of_memory\n");
			return (AUTH_FAILED);
		}
		auth->svc_ah_ops = &svc_auth_gss_ops;
		SVCAUTH_PRIVATE(auth) = gd;
		rqst->rq_xprt->xp_auth = auth;
	}
	else gd = SVCAUTH_PRIVATE(rqst->rq_xprt->xp_auth);

	log_debug("xp_auth=%p, gd=%p", rqst->rq_xprt->xp_auth, gd);

	/* Deserialize client credentials. */
	if (rqst->rq_cred.oa_length <= 0)
		return (AUTH_BADCRED);

	gc = (struct rpc_gss_cred *)rqst->rq_clntcred;
	memset(gc, 0, sizeof(*gc));

	log_debug("calling xdrmem_create()");
	log_debug("oa_base=%p, oa_length=%u", rqst->rq_cred.oa_base,
		  rqst->rq_cred.oa_length);
	xdrmem_create(&xdrs, rqst->rq_cred.oa_base,
		      rqst->rq_cred.oa_length, XDR_DECODE);
	log_debug("xdrmem_create() returned");

	if (!xdr_rpc_gss_cred(&xdrs, gc)) {
		log_debug("xdr_rpc_gss_cred() failed");
		XDR_DESTROY(&xdrs);
		return (AUTH_BADCRED);
	}
	XDR_DESTROY(&xdrs);

	retstat = AUTH_FAILED;

#define ret_freegc(code) do { retstat = code; goto freegc; } while (0)

	/* Check version. */
	if (gc->gc_v != RPCSEC_GSS_VERSION)
		ret_freegc (AUTH_BADCRED);

	/* Check RPCSEC_GSS service. */
	if (gc->gc_svc != RPCSEC_GSS_SVC_NONE &&
	    gc->gc_svc != RPCSEC_GSS_SVC_INTEGRITY &&
	    gc->gc_svc != RPCSEC_GSS_SVC_PRIVACY)
		ret_freegc (AUTH_BADCRED);

	/* Check sequence number. */
	if (gd->established) {
		if (gc->gc_seq > MAXSEQ)
			ret_freegc (RPCSEC_GSS_CTXPROBLEM);

		if ((offset = gd->seqlast - gc->gc_seq) < 0) {
			gd->seqlast = gc->gc_seq;
			offset = 0 - offset;
			gd->seqmask <<= offset;
			offset = 0;
		} else if ((u_int)offset >= gd->win ||
			   (gd->seqmask & (1 << offset))) {
			*no_dispatch = 1;
			ret_freegc (RPCSEC_GSS_CTXPROBLEM);
		}
		gd->seq = gc->gc_seq;
		gd->seqmask |= (1 << offset);
	}

	if (gd->established) {
		rqst->rq_clntname = (char *)gd->client_name;
		rqst->rq_svccred = (char *)gd->ctx;
	}

	/* Handle RPCSEC_GSS control procedure. */
	switch (gc->gc_proc) {

	case RPCSEC_GSS_INIT:
	case RPCSEC_GSS_CONTINUE_INIT:
		if (rqst->rq_proc != NULLPROC)
			ret_freegc (AUTH_FAILED);		/* XXX ? */

		if (!svcauth_gss_acquire_cred())
			ret_freegc (AUTH_FAILED);

		if (!svcauth_gss_accept_sec_context(rqst, &gr))
			ret_freegc (AUTH_REJECTEDCRED);
 
 		if (!svcauth_gss_nextverf(rqst, htonl(gr.gr_win))) {
 			gss_release_buffer(&min_stat, &gr.gr_token);
 			ret_freegc (AUTH_FAILED);
 		}
 		*no_dispatch = TRUE;

		call_stat = svc_sendreply(rqst->rq_xprt, xdr_rpc_gss_init_res,
					  (caddr_t)&gr);
 
 		gss_release_buffer(&min_stat, &gr.gr_token);
 		gss_release_buffer(&min_stat, &gd->checksum);
 		if (!call_stat)
 			ret_freegc (AUTH_FAILED);
 
		if (gr.gr_major == GSS_S_COMPLETE)
			gd->established = TRUE;

		break;

	case RPCSEC_GSS_DATA:
		if (!svcauth_gss_validate(rqst, gd, msg))
			ret_freegc (RPCSEC_GSS_CREDPROBLEM);

		if (!svcauth_gss_nextverf(rqst, htonl(gc->gc_seq)))
 			ret_freegc (AUTH_FAILED);
		break;

	case RPCSEC_GSS_DESTROY:
		if (rqst->rq_proc != NULLPROC)
			ret_freegc (AUTH_FAILED);		/* XXX ? */

		if (!svcauth_gss_validate(rqst, gd, msg))
			ret_freegc (RPCSEC_GSS_CREDPROBLEM);

		if (!svcauth_gss_nextverf(rqst, htonl(gc->gc_seq)))
			ret_freegc (AUTH_FAILED);

		*no_dispatch = TRUE;

		call_stat = svc_sendreply(rqst->rq_xprt,
					  xdr_void, (caddr_t)NULL);

		log_debug("sendreply in destroy: %d", call_stat);

		if (!svcauth_gss_release_cred())
			ret_freegc (AUTH_FAILED);

		SVCAUTH_DESTROY(rqst->rq_xprt->xp_auth);
		rqst->rq_xprt->xp_auth = &svc_auth_none;

		break;

	default:
		ret_freegc (AUTH_REJECTEDCRED);
		break;
	}
	retstat = AUTH_OK;
freegc:
	xdr_free(xdr_rpc_gss_cred, gc);
	log_debug("returning %d from svcauth_gss()", retstat);
	return (retstat);
}
