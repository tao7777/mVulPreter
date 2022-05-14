stf_status ikev2parent_inI1outR1(struct msg_digest *md)
{
	struct state *st = md->st;
	lset_t policy = POLICY_IKEV2_ALLOW;
	struct connection *c = find_host_connection(&md->iface->ip_addr,
						    md->iface->port,
						    &md->sender,
						    md->sender_port,
						    POLICY_IKEV2_ALLOW);

	/* retrieve st->st_gi */

#if 0
	if (c == NULL) {
		/*
		 * make up a policy from the thing that was proposed, and see
		 * if we can find a connection with that policy.
		 */

		pb_stream pre_sa_pbs = sa_pd->pbs;
		policy = preparse_isakmp_sa_body(&pre_sa_pbs);
		c = find_host_connection(&md->iface->ip_addr, pluto_port,
					 (ip_address*)NULL, md->sender_port,
					 policy);

	}
#endif

	if (c == NULL) {

		/* See if a wildcarded connection can be found.
		 * We cannot pick the right connection, so we're making a guess.
		 * All Road Warrior connections are fair game:
		 * we pick the first we come across (if any).
		 * If we don't find any, we pick the first opportunistic
		 * with the smallest subnet that includes the peer.
		 * There is, of course, no necessary relationship between
		 * an Initiator's address and that of its client,
		 * but Food Groups kind of assumes one.
		 */
		{
			struct connection *d;
			d = find_host_connection(&md->iface->ip_addr,
						 pluto_port,
						 (ip_address*)NULL,
						 md->sender_port, policy);

			for (; d != NULL; d = d->hp_next) {
				if (d->kind == CK_GROUP) {
					/* ignore */
				} else {
					if (d->kind == CK_TEMPLATE &&
					    !(d->policy & POLICY_OPPO)) {
						/* must be Road Warrior: we have a winner */
						c = d;
						break;
					}

					/* Opportunistic or Shunt: pick tightest match */
					if (addrinsubnet(&md->sender,
							 &d->spd.that.client)
					    &&
					    (c == NULL ||
					     !subnetinsubnet(&c->spd.that.
							     client,
							     &d->spd.that.
							     client)))
						c = d;
				}
			}
		}
		if (c == NULL) {
			loglog(RC_LOG_SERIOUS, "initial parent SA message received on %s:%u"
			       " but no connection has been authorized%s%s",
			       ip_str(
				       &md->iface->ip_addr),
			       ntohs(portof(&md->iface->ip_addr)),
			       (policy != LEMPTY) ? " with policy=" : "",
			       (policy !=
				LEMPTY) ? bitnamesof(sa_policy_bit_names,
						     policy) : "");
			return STF_FAIL + v2N_NO_PROPOSAL_CHOSEN;
		}
		if (c->kind != CK_TEMPLATE) {
			loglog(RC_LOG_SERIOUS, "initial parent SA message received on %s:%u"
			       " but \"%s\" forbids connection",
			       ip_str(
				       &md->iface->ip_addr), pluto_port,
			       c->name);
			return STF_FAIL + v2N_NO_PROPOSAL_CHOSEN;
		}
		c = rw_instantiate(c, &md->sender, NULL, NULL);

	} else {
		/* we found a non-wildcard conn. double check if it needs instantiation anyway (eg vnet=) */
		/* vnet=/vhost= should have set CK_TEMPLATE on connection loading */
		if ((c->kind == CK_TEMPLATE) && c->spd.that.virt) {
			DBG(DBG_CONTROL,
			    DBG_log(
				    "local endpoint has virt (vnet/vhost) set without wildcards - needs instantiation"));
			c = rw_instantiate(c, &md->sender, NULL, NULL);
		} else if ((c->kind == CK_TEMPLATE) &&
			   (c->policy & POLICY_IKEV2_ALLOW_NARROWING)) {
			DBG(DBG_CONTROL,
			    DBG_log(
				    "local endpoint has narrowing=yes - needs instantiation"));
			c = rw_instantiate(c, &md->sender, NULL, NULL);
		}
	}

	DBG_log("found connection: %s\n", c ? c->name : "<none>");

	if (!st) {
		st = new_state();
		/* set up new state */
		memcpy(st->st_icookie, md->hdr.isa_icookie, COOKIE_SIZE);
		/* initialize_new_state expects valid icookie/rcookie values, so create it now */
		get_cookie(FALSE, st->st_rcookie, COOKIE_SIZE, &md->sender);
		initialize_new_state(st, c, policy, 0, NULL_FD,
				     pcim_stranger_crypto);
		st->st_ikev2 = TRUE;
		change_state(st, STATE_PARENT_R1);
		st->st_msgid_lastack = INVALID_MSGID;
		st->st_msgid_nextuse = 0;

		md->st = st;
		md->from_state = STATE_IKEv2_BASE;
	}

	/* check,as a responder, are we under dos attack or not
	 * if yes go to 6 message exchange mode. it is a config option for now.
	 * TBD set force_busy dynamically
	 * Paul: Can we check for STF_TOOMUCHCRYPTO ?
	 */
	if (force_busy == TRUE) {
		u_char dcookie[SHA1_DIGEST_SIZE];
		chunk_t dc;
		ikev2_get_dcookie( dcookie, st->st_ni, &md->sender,
				   st->st_icookie);
		dc.ptr = dcookie;
		dc.len = SHA1_DIGEST_SIZE;

		/* check if I1 packet contian KE and a v2N payload with type COOKIE */
		if ( md->chain[ISAKMP_NEXT_v2KE] &&
		     md->chain[ISAKMP_NEXT_v2N] &&
		     (md->chain[ISAKMP_NEXT_v2N]->payload.v2n.isan_type ==
		      v2N_COOKIE)) {
			u_int8_t spisize;
			const pb_stream *dc_pbs;
			chunk_t blob;
			DBG(DBG_CONTROLMORE,
			    DBG_log("received a DOS cookie in I1 verify it"));
			/* we received dcookie we send earlier verify it */
			spisize =
				md->chain[ISAKMP_NEXT_v2N]->payload.v2n.
				isan_spisize;
			dc_pbs = &md->chain[ISAKMP_NEXT_v2N]->pbs;
			blob.ptr = dc_pbs->cur + spisize;
			blob.len = pbs_left(dc_pbs) - spisize;
			DBG(DBG_CONTROLMORE,
			    DBG_dump_chunk("dcookie received in I1 Packet",
					   blob);
			    DBG_dump("dcookie computed", dcookie,
				     SHA1_DIGEST_SIZE));

			if (memcmp(blob.ptr, dcookie, SHA1_DIGEST_SIZE) != 0) {
				libreswan_log(
					"mismatch in DOS v2N_COOKIE,send a new one");
				SEND_NOTIFICATION_AA(v2N_COOKIE, &dc);
				return STF_FAIL + v2N_INVALID_IKE_SPI;
			}
			DBG(DBG_CONTROLMORE,
			    DBG_log("dcookie received match with computed one"));
		} else {
			/* we are under DOS attack I1 contains no DOS COOKIE */
			DBG(DBG_CONTROLMORE,
			    DBG_log(
				    "busy mode on. receieved I1 without a valid dcookie");
			    DBG_log("send a dcookie and forget this state"));
			SEND_NOTIFICATION_AA(v2N_COOKIE, &dc);
			return STF_FAIL;
		}
	} else {
		DBG(DBG_CONTROLMORE,
		    DBG_log("will not send/process a dcookie"));

	}

	/*
	 * We have to agree to the DH group before we actually know who
	 * we are talking to.   If we support the group, we use it.
	 *
	 * It is really too hard here to go through all the possible policies
	 * that might permit this group.  If we think we are being DOS'ed
	 * then we should demand a cookie.
 	 */
 	{
 		struct ikev2_ke *ke;
		char fromname[ADDRTOT_BUF];
		addrtot(&md->sender, 0, fromname, ADDRTOT_BUF);

		if (!md->chain[ISAKMP_NEXT_v2KE]) {
			/* is this a notify? If so, log it */
			if(md->chain[ISAKMP_NEXT_v2N]) {
				libreswan_log("Received Notify(%d): %s",
					md->chain[ISAKMP_NEXT_v2N]->payload.v2n.isan_type,
					enum_name(&ikev2_notify_names,
						md->chain[ISAKMP_NEXT_v2N]->payload.v2n.isan_type));
			}
			libreswan_log(
				"rejecting I1 from %s:%u, no KE payload present",
				fromname, md->sender_port);
			return STF_FAIL + v2N_INVALID_KE_PAYLOAD;
		}
 		ke = &md->chain[ISAKMP_NEXT_v2KE]->payload.v2ke;
 
 		st->st_oakley.group = lookup_group(ke->isak_group);
 		if (st->st_oakley.group == NULL) {
 			libreswan_log(
 				"rejecting I1 from %s:%u, invalid DH group=%u",
 				fromname, md->sender_port,
 				ke->isak_group);
			return STF_FAIL + v2N_INVALID_KE_PAYLOAD;
 		}
 	}
 
	/* now. we need to go calculate the nonce, and the KE */
	{
		struct ke_continuation *ke = alloc_thing(
			struct ke_continuation,
			"ikev2_inI1outR1 KE");
		stf_status e;

		ke->md = md;
		set_suspended(st, ke->md);

		if (!st->st_sec_in_use) {
			pcrc_init(&ke->ke_pcrc);
			ke->ke_pcrc.pcrc_func =
				ikev2_parent_inI1outR1_continue;
			e = build_ke(&ke->ke_pcrc, st, st->st_oakley.group,
				     pcim_stranger_crypto);
			if (e != STF_SUSPEND && e != STF_INLINE) {
				loglog(RC_CRYPTOFAILED, "system too busy");
				delete_state(st);
			}
		} else {
			e =
				ikev2_parent_inI1outR1_tail((struct
							     pluto_crypto_req_cont
							     *)ke,
							    NULL);
		}

		reset_globals();

		return e;
	}
}
