static void ikev2_parent_inI2outR2_continue(struct pluto_crypto_req_cont *pcrc,
					    struct pluto_crypto_req *r,
					    err_t ugh)
{
	struct dh_continuation *dh = (struct dh_continuation *)pcrc;
	struct msg_digest *md = dh->md;
	struct state *const st = md->st;
	stf_status e;

	DBG(DBG_CONTROLMORE,
	    DBG_log("ikev2 parent inI2outR2: calculating g^{xy}, sending R2"));

	if (st == NULL) {
		loglog(RC_LOG_SERIOUS,
		       "%s: Request was disconnected from state",
		       __FUNCTION__);
		if (dh->md)
			release_md(dh->md);
		return;
	}

	/* XXX should check out ugh */
	passert(ugh == NULL);
	passert(cur_state == NULL);
	passert(st != NULL);

	passert(st->st_suspended_md == dh->md);
	set_suspended(st, NULL); /* no longer connected or suspended */

	set_cur_state(st);

	st->st_calculating = FALSE;

	e = ikev2_parent_inI2outR2_tail(pcrc, r);
	if ( e > STF_FAIL) {
		/* we do not send a notify because we are the initiator that could be responding to an error notification */
		int v2_notify_num = e - STF_FAIL;
		DBG_log(
			"ikev2_parent_inI2outR2_tail returned STF_FAIL with %s",
			enum_name(&ikev2_notify_names, v2_notify_num));
	} else if ( e != STF_OK) {
		DBG_log("ikev2_parent_inI2outR2_tail returned %s",
			enum_name(&stfstatus_name, e));
	}

	if (dh->md != NULL) {
		complete_v2_state_transition(&dh->md, e);
		if (dh->md)
 			release_md(dh->md);
 	}
 	reset_globals();
 }
