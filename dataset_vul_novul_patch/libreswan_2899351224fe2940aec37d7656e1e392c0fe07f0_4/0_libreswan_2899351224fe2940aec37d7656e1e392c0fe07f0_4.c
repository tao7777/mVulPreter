stf_status ikev2parent_inI2outR2(struct msg_digest *md)
{
	struct state *st = md->st;

	/* struct connection *c = st->st_connection; */

	/*
	 * the initiator sent us an encrypted payload. We need to calculate
	 * our g^xy, and skeyseed values, and then decrypt the payload.
	 */

	DBG(DBG_CONTROLMORE,
	    DBG_log(
		    "ikev2 parent inI2outR2: calculating g^{xy} in order to decrypt I2"));

 	/* verify that there is in fact an encrypted payload */
 	if (!md->chain[ISAKMP_NEXT_v2E]) {
 		libreswan_log("R2 state should receive an encrypted payload");
		reset_globals(); /* XXX suspicious - why was this deemed neccessary? */
 		return STF_FATAL;
 	}
 
	/* now. we need to go calculate the g^xy */
	{
		struct dh_continuation *dh = alloc_thing(
			struct dh_continuation,
			"ikev2_inI2outR2 KE");
		stf_status e;

		dh->md = md;
		set_suspended(st, dh->md);

		pcrc_init(&dh->dh_pcrc);
		dh->dh_pcrc.pcrc_func = ikev2_parent_inI2outR2_continue;
		e = start_dh_v2(&dh->dh_pcrc, st, st->st_import, RESPONDER,
				st->st_oakley.groupnum);
		if (e != STF_SUSPEND && e != STF_INLINE) {
			loglog(RC_CRYPTOFAILED, "system too busy");
			delete_state(st);
		}

		reset_globals();

		return e;
	}
}
