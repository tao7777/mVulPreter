sctp_disposition_t sctp_sf_do_5_1D_ce(struct net *net,
				      const struct sctp_endpoint *ep,
				      const struct sctp_association *asoc,
				      const sctp_subtype_t type, void *arg,
				      sctp_cmd_seq_t *commands)
{
	struct sctp_chunk *chunk = arg;
	struct sctp_association *new_asoc;
	sctp_init_chunk_t *peer_init;
	struct sctp_chunk *repl;
	struct sctp_ulpevent *ev, *ai_ev = NULL;
	int error = 0;
	struct sctp_chunk *err_chk_p;
	struct sock *sk;

	/* If the packet is an OOTB packet which is temporarily on the
	 * control endpoint, respond with an ABORT.
	 */
	if (ep == sctp_sk(net->sctp.ctl_sock)->ep) {
		SCTP_INC_STATS(net, SCTP_MIB_OUTOFBLUES);
		return sctp_sf_tabort_8_4_8(net, ep, asoc, type, arg, commands);
	}

	/* Make sure that the COOKIE_ECHO chunk has a valid length.
	 * In this case, we check that we have enough for at least a
	 * chunk header.  More detailed verification is done
	 * in sctp_unpack_cookie().
	 */
	if (!sctp_chunk_length_valid(chunk, sizeof(sctp_chunkhdr_t)))
		return sctp_sf_pdiscard(net, ep, asoc, type, arg, commands);

	/* If the endpoint is not listening or if the number of associations
	 * on the TCP-style socket exceed the max backlog, respond with an
	 * ABORT.
	 */
	sk = ep->base.sk;
	if (!sctp_sstate(sk, LISTENING) ||
	    (sctp_style(sk, TCP) && sk_acceptq_is_full(sk)))
		return sctp_sf_tabort_8_4_8(net, ep, asoc, type, arg, commands);

	/* "Decode" the chunk.  We have no optional parameters so we
	 * are in good shape.
	 */
	chunk->subh.cookie_hdr =
		(struct sctp_signed_cookie *)chunk->skb->data;
	if (!pskb_pull(chunk->skb, ntohs(chunk->chunk_hdr->length) -
					 sizeof(sctp_chunkhdr_t)))
		goto nomem;

	/* 5.1 D) Upon reception of the COOKIE ECHO chunk, Endpoint
	 * "Z" will reply with a COOKIE ACK chunk after building a TCB
	 * and moving to the ESTABLISHED state.
	 */
	new_asoc = sctp_unpack_cookie(ep, asoc, chunk, GFP_ATOMIC, &error,
				      &err_chk_p);

	/* FIXME:
	 * If the re-build failed, what is the proper error path
	 * from here?
	 *
	 * [We should abort the association. --piggy]
	 */
	if (!new_asoc) {
		/* FIXME: Several errors are possible.  A bad cookie should
		 * be silently discarded, but think about logging it too.
		 */
		switch (error) {
		case -SCTP_IERROR_NOMEM:
			goto nomem;

		case -SCTP_IERROR_STALE_COOKIE:
			sctp_send_stale_cookie_err(net, ep, asoc, chunk, commands,
						   err_chk_p);
			return sctp_sf_pdiscard(net, ep, asoc, type, arg, commands);

		case -SCTP_IERROR_BAD_SIG:
		default:
			return sctp_sf_pdiscard(net, ep, asoc, type, arg, commands);
		}
	}


	/* Delay state machine commands until later.
	 *
	 * Re-build the bind address for the association is done in
	 * the sctp_unpack_cookie() already.
	 */
	/* This is a brand-new association, so these are not yet side
	 * effects--it is safe to run them here.
	 */
	peer_init = &chunk->subh.cookie_hdr->c.peer_init[0];

	if (!sctp_process_init(new_asoc, chunk,
			       &chunk->subh.cookie_hdr->c.peer_addr,
			       peer_init, GFP_ATOMIC))
		goto nomem_init;

	/* SCTP-AUTH:  Now that we've populate required fields in
	 * sctp_process_init, set up the assocaition shared keys as
	 * necessary so that we can potentially authenticate the ACK
	 */
	error = sctp_auth_asoc_init_active_key(new_asoc, GFP_ATOMIC);
	if (error)
		goto nomem_init;

	/* SCTP-AUTH:  auth_chunk pointer is only set when the cookie-echo
	 * is supposed to be authenticated and we have to do delayed
	 * authentication.  We've just recreated the association using
	 * the information in the cookie and now it's much easier to
	 * do the authentication.
	 */
	if (chunk->auth_chunk) {
 		struct sctp_chunk auth;
 		sctp_ierror_t ret;
 
		/* Make sure that we and the peer are AUTH capable */
		if (!net->sctp.auth_enable || !new_asoc->peer.auth_capable) {
			kfree_skb(chunk->auth_chunk);
			sctp_association_free(new_asoc);
			return sctp_sf_pdiscard(net, ep, asoc, type, arg, commands);
		}

 		/* set-up our fake chunk so that we can process it */
 		auth.skb = chunk->auth_chunk;
 		auth.asoc = chunk->asoc;
		auth.sctp_hdr = chunk->sctp_hdr;
		auth.chunk_hdr = (sctp_chunkhdr_t *)skb_push(chunk->auth_chunk,
					    sizeof(sctp_chunkhdr_t));
		skb_pull(chunk->auth_chunk, sizeof(sctp_chunkhdr_t));
		auth.transport = chunk->transport;

		ret = sctp_sf_authenticate(net, ep, new_asoc, type, &auth);

		/* We can now safely free the auth_chunk clone */
		kfree_skb(chunk->auth_chunk);

		if (ret != SCTP_IERROR_NO_ERROR) {
			sctp_association_free(new_asoc);
			return sctp_sf_pdiscard(net, ep, asoc, type, arg, commands);
		}
	}

	repl = sctp_make_cookie_ack(new_asoc, chunk);
	if (!repl)
		goto nomem_init;

	/* RFC 2960 5.1 Normal Establishment of an Association
	 *
	 * D) IMPLEMENTATION NOTE: An implementation may choose to
	 * send the Communication Up notification to the SCTP user
	 * upon reception of a valid COOKIE ECHO chunk.
	 */
	ev = sctp_ulpevent_make_assoc_change(new_asoc, 0, SCTP_COMM_UP, 0,
					     new_asoc->c.sinit_num_ostreams,
					     new_asoc->c.sinit_max_instreams,
					     NULL, GFP_ATOMIC);
	if (!ev)
		goto nomem_ev;

	/* Sockets API Draft Section 5.3.1.6
	 * When a peer sends a Adaptation Layer Indication parameter , SCTP
	 * delivers this notification to inform the application that of the
	 * peers requested adaptation layer.
	 */
	if (new_asoc->peer.adaptation_ind) {
		ai_ev = sctp_ulpevent_make_adaptation_indication(new_asoc,
							    GFP_ATOMIC);
		if (!ai_ev)
			goto nomem_aiev;
	}

	/* Add all the state machine commands now since we've created
	 * everything.  This way we don't introduce memory corruptions
	 * during side-effect processing and correclty count established
	 * associations.
	 */
	sctp_add_cmd_sf(commands, SCTP_CMD_NEW_ASOC, SCTP_ASOC(new_asoc));
	sctp_add_cmd_sf(commands, SCTP_CMD_NEW_STATE,
			SCTP_STATE(SCTP_STATE_ESTABLISHED));
	SCTP_INC_STATS(net, SCTP_MIB_CURRESTAB);
	SCTP_INC_STATS(net, SCTP_MIB_PASSIVEESTABS);
	sctp_add_cmd_sf(commands, SCTP_CMD_HB_TIMERS_START, SCTP_NULL());

	if (new_asoc->timeouts[SCTP_EVENT_TIMEOUT_AUTOCLOSE])
		sctp_add_cmd_sf(commands, SCTP_CMD_TIMER_START,
				SCTP_TO(SCTP_EVENT_TIMEOUT_AUTOCLOSE));

	/* This will send the COOKIE ACK */
	sctp_add_cmd_sf(commands, SCTP_CMD_REPLY, SCTP_CHUNK(repl));

	/* Queue the ASSOC_CHANGE event */
	sctp_add_cmd_sf(commands, SCTP_CMD_EVENT_ULP, SCTP_ULPEVENT(ev));

	/* Send up the Adaptation Layer Indication event */
	if (ai_ev)
		sctp_add_cmd_sf(commands, SCTP_CMD_EVENT_ULP,
				SCTP_ULPEVENT(ai_ev));

	return SCTP_DISPOSITION_CONSUME;

nomem_aiev:
	sctp_ulpevent_free(ev);
nomem_ev:
	sctp_chunk_free(repl);
nomem_init:
	sctp_association_free(new_asoc);
nomem:
	return SCTP_DISPOSITION_NOMEM;
}
