ssh_packet_read_poll2(struct ssh *ssh, u_char *typep, u_int32_t *seqnr_p)
{
	struct session_state *state = ssh->state;
	u_int padlen, need;
	u_char *cp, macbuf[SSH_DIGEST_MAX_LENGTH];
	u_int maclen, aadlen = 0, authlen = 0, block_size;
	struct sshenc *enc   = NULL;
	struct sshmac *mac   = NULL;
	struct sshcomp *comp = NULL;
	int r;

	*typep = SSH_MSG_NONE;

	if (state->packet_discard)
		return 0;

	if (state->newkeys[MODE_IN] != NULL) {
		enc  = &state->newkeys[MODE_IN]->enc;
		mac  = &state->newkeys[MODE_IN]->mac;
		comp = &state->newkeys[MODE_IN]->comp;
		/* disable mac for authenticated encryption */
		if ((authlen = cipher_authlen(enc->cipher)) != 0)
			mac = NULL;
	}
	maclen = mac && mac->enabled ? mac->mac_len : 0;
	block_size = enc ? enc->block_size : 8;
	aadlen = (mac && mac->enabled && mac->etm) || authlen ? 4 : 0;

	if (aadlen && state->packlen == 0) {
		if (cipher_get_length(&state->receive_context,
		    &state->packlen, state->p_read.seqnr,
		    sshbuf_ptr(state->input), sshbuf_len(state->input)) != 0)
			return 0;
		if (state->packlen < 1 + 4 ||
		    state->packlen > PACKET_MAX_SIZE) {
#ifdef PACKET_DEBUG
			sshbuf_dump(state->input, stderr);
#endif
 			logit("Bad packet length %u.", state->packlen);
 			if ((r = sshpkt_disconnect(ssh, "Packet corrupt")) != 0)
 				return r;
			return SSH_ERR_CONN_CORRUPT;
 		}
 		sshbuf_reset(state->incoming_packet);
 	} else if (state->packlen == 0) {
		 * check if input size is less than the cipher block size,
		 * decrypt first block and extract length of incoming packet
		 */
		if (sshbuf_len(state->input) < block_size)
			return 0;
		sshbuf_reset(state->incoming_packet);
		if ((r = sshbuf_reserve(state->incoming_packet, block_size,
		    &cp)) != 0)
			goto out;
		if ((r = cipher_crypt(&state->receive_context,
		    state->p_send.seqnr, cp, sshbuf_ptr(state->input),
		    block_size, 0, 0)) != 0)
			goto out;
		state->packlen = PEEK_U32(sshbuf_ptr(state->incoming_packet));
		if (state->packlen < 1 + 4 ||
		    state->packlen > PACKET_MAX_SIZE) {
#ifdef PACKET_DEBUG
			fprintf(stderr, "input: \n");
			sshbuf_dump(state->input, stderr);
			fprintf(stderr, "incoming_packet: \n");
			sshbuf_dump(state->incoming_packet, stderr);
#endif
			logit("Bad packet length %u.", state->packlen);
			return ssh_packet_start_discard(ssh, enc, mac,
			    state->packlen, PACKET_MAX_SIZE);
		}
		if ((r = sshbuf_consume(state->input, block_size)) != 0)
			goto out;
	}
	DBG(debug("input: packet len %u", state->packlen+4));

	if (aadlen) {
		/* only the payload is encrypted */
		need = state->packlen;
	} else {
		/*
		 * the payload size and the payload are encrypted, but we
		 * have a partial packet of block_size bytes
		 */
		need = 4 + state->packlen - block_size;
	}
	DBG(debug("partial packet: block %d, need %d, maclen %d, authlen %d,"
	    " aadlen %d", block_size, need, maclen, authlen, aadlen));
	if (need % block_size != 0) {
		logit("padding error: need %d block %d mod %d",
		    need, block_size, need % block_size);
		return ssh_packet_start_discard(ssh, enc, mac,
		    state->packlen, PACKET_MAX_SIZE - block_size);
	}
	/*
	 * check if the entire packet has been received and
	 * decrypt into incoming_packet:
	 * 'aadlen' bytes are unencrypted, but authenticated.
	 * 'need' bytes are encrypted, followed by either
	 * 'authlen' bytes of authentication tag or
	 * 'maclen' bytes of message authentication code.
	 */
	if (sshbuf_len(state->input) < aadlen + need + authlen + maclen)
		return 0;
#ifdef PACKET_DEBUG
	fprintf(stderr, "read_poll enc/full: ");
	sshbuf_dump(state->input, stderr);
#endif
	/* EtM: compute mac over encrypted input */
	if (mac && mac->enabled && mac->etm) {
		if ((r = mac_compute(mac, state->p_read.seqnr,
		    sshbuf_ptr(state->input), aadlen + need,
		    macbuf, sizeof(macbuf))) != 0)
			goto out;
	}
	if ((r = sshbuf_reserve(state->incoming_packet, aadlen + need,
	    &cp)) != 0)
		goto out;
	if ((r = cipher_crypt(&state->receive_context, state->p_read.seqnr, cp,
	    sshbuf_ptr(state->input), need, aadlen, authlen)) != 0)
		goto out;
	if ((r = sshbuf_consume(state->input, aadlen + need + authlen)) != 0)
		goto out;
	/*
	 * compute MAC over seqnr and packet,
	 * increment sequence number for incoming packet
	 */
	if (mac && mac->enabled) {
		if (!mac->etm)
			if ((r = mac_compute(mac, state->p_read.seqnr,
			    sshbuf_ptr(state->incoming_packet),
			    sshbuf_len(state->incoming_packet),
			    macbuf, sizeof(macbuf))) != 0)
				goto out;
		if (timingsafe_bcmp(macbuf, sshbuf_ptr(state->input),
		    mac->mac_len) != 0) {
			logit("Corrupted MAC on input.");
			if (need > PACKET_MAX_SIZE)
				return SSH_ERR_INTERNAL_ERROR;
			return ssh_packet_start_discard(ssh, enc, mac,
			    state->packlen, PACKET_MAX_SIZE - need);
		}

		DBG(debug("MAC #%d ok", state->p_read.seqnr));
		if ((r = sshbuf_consume(state->input, mac->mac_len)) != 0)
			goto out;
	}
	if (seqnr_p != NULL)
		*seqnr_p = state->p_read.seqnr;
	if (++state->p_read.seqnr == 0)
		logit("incoming seqnr wraps around");
	if (++state->p_read.packets == 0)
		if (!(ssh->compat & SSH_BUG_NOREKEY))
			return SSH_ERR_NEED_REKEY;
	state->p_read.blocks += (state->packlen + 4) / block_size;
	state->p_read.bytes += state->packlen + 4;

	/* get padlen */
	padlen = sshbuf_ptr(state->incoming_packet)[4];
	DBG(debug("input: padlen %d", padlen));
	if (padlen < 4)	{
		if ((r = sshpkt_disconnect(ssh,
		    "Corrupted padlen %d on input.", padlen)) != 0 ||
		    (r = ssh_packet_write_wait(ssh)) != 0)
			return r;
		return SSH_ERR_CONN_CORRUPT;
	}

	/* skip packet size + padlen, discard padding */
	if ((r = sshbuf_consume(state->incoming_packet, 4 + 1)) != 0 ||
	    ((r = sshbuf_consume_end(state->incoming_packet, padlen)) != 0))
		goto out;

	DBG(debug("input: len before de-compress %zd",
	    sshbuf_len(state->incoming_packet)));
	if (comp && comp->enabled) {
		sshbuf_reset(state->compression_buffer);
		if ((r = uncompress_buffer(ssh, state->incoming_packet,
		    state->compression_buffer)) != 0)
			goto out;
		sshbuf_reset(state->incoming_packet);
		if ((r = sshbuf_putb(state->incoming_packet,
		    state->compression_buffer)) != 0)
			goto out;
		DBG(debug("input: len after de-compress %zd",
		    sshbuf_len(state->incoming_packet)));
	}
	/*
	 * get packet type, implies consume.
	 * return length of payload (without type field)
	 */
	if ((r = sshbuf_get_u8(state->incoming_packet, typep)) != 0)
		goto out;
	if (*typep < SSH2_MSG_MIN || *typep >= SSH2_MSG_LOCAL_MIN) {
		if ((r = sshpkt_disconnect(ssh,
		    "Invalid ssh2 packet type: %d", *typep)) != 0 ||
		    (r = ssh_packet_write_wait(ssh)) != 0)
			return r;
		return SSH_ERR_PROTOCOL_ERROR;
	}
	if (*typep == SSH2_MSG_NEWKEYS)
		r = ssh_set_newkeys(ssh, MODE_IN);
	else if (*typep == SSH2_MSG_USERAUTH_SUCCESS && !state->server_side)
		r = ssh_packet_enable_delayed_compress(ssh);
	else
		r = 0;
#ifdef PACKET_DEBUG
	fprintf(stderr, "read/plain[%d]:\r\n", *typep);
	sshbuf_dump(state->incoming_packet, stderr);
#endif
	/* reset for next packet */
	state->packlen = 0;
 out:
	return r;
}
