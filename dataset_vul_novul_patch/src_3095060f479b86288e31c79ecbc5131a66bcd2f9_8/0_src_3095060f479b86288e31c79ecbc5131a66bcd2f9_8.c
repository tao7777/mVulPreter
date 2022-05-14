ssh_packet_set_state(struct ssh *ssh, struct sshbuf *m)
{
	struct session_state *state = ssh->state;
	const u_char *ssh1key, *ivin, *ivout, *keyin, *keyout, *input, *output;
	size_t ssh1keylen, rlen, slen, ilen, olen;
	int r;
	u_int ssh1cipher = 0;

	if (!compat20) {
		if ((r = sshbuf_get_u32(m, &state->remote_protocol_flags)) != 0 ||
		    (r = sshbuf_get_u32(m, &ssh1cipher)) != 0 ||
		    (r = sshbuf_get_string_direct(m, &ssh1key, &ssh1keylen)) != 0 ||
		    (r = sshbuf_get_string_direct(m, &ivout, &slen)) != 0 ||
		    (r = sshbuf_get_string_direct(m, &ivin, &rlen)) != 0)
			return r;
		if (ssh1cipher > INT_MAX)
			return SSH_ERR_KEY_UNKNOWN_CIPHER;
		ssh_packet_set_encryption_key(ssh, ssh1key, ssh1keylen,
		    (int)ssh1cipher);
		if (cipher_get_keyiv_len(state->send_context) != (int)slen ||
		    cipher_get_keyiv_len(state->receive_context) != (int)rlen)
			return SSH_ERR_INVALID_FORMAT;
		if ((r = cipher_set_keyiv(state->send_context, ivout)) != 0 ||
		    (r = cipher_set_keyiv(state->receive_context, ivin)) != 0)
			return r;
	} else {
		if ((r = kex_from_blob(m, &ssh->kex)) != 0 ||
		    (r = newkeys_from_blob(m, ssh, MODE_OUT)) != 0 ||
		    (r = newkeys_from_blob(m, ssh, MODE_IN)) != 0 ||
		    (r = sshbuf_get_u64(m, &state->rekey_limit)) != 0 ||
		    (r = sshbuf_get_u32(m, &state->rekey_interval)) != 0 ||
		    (r = sshbuf_get_u32(m, &state->p_send.seqnr)) != 0 ||
		    (r = sshbuf_get_u64(m, &state->p_send.blocks)) != 0 ||
		    (r = sshbuf_get_u32(m, &state->p_send.packets)) != 0 ||
		    (r = sshbuf_get_u64(m, &state->p_send.bytes)) != 0 ||
		    (r = sshbuf_get_u32(m, &state->p_read.seqnr)) != 0 ||
		    (r = sshbuf_get_u64(m, &state->p_read.blocks)) != 0 ||
		    (r = sshbuf_get_u32(m, &state->p_read.packets)) != 0 ||
		    (r = sshbuf_get_u64(m, &state->p_read.bytes)) != 0)
			return r;
		/*
		 * We set the time here so that in post-auth privsep slave we
		 * count from the completion of the authentication.
		 */
		state->rekey_time = monotime();
		/* XXX ssh_set_newkeys overrides p_read.packets? XXX */
		if ((r = ssh_set_newkeys(ssh, MODE_IN)) != 0 ||
		    (r = ssh_set_newkeys(ssh, MODE_OUT)) != 0)
			return r;
	}
	if ((r = sshbuf_get_string_direct(m, &keyout, &slen)) != 0 ||
	    (r = sshbuf_get_string_direct(m, &keyin, &rlen)) != 0)
		return r;
	if (cipher_get_keycontext(state->send_context, NULL) != (int)slen ||
	    cipher_get_keycontext(state->receive_context, NULL) != (int)rlen)
		return SSH_ERR_INVALID_FORMAT;
 	cipher_set_keycontext(state->send_context, keyout);
 	cipher_set_keycontext(state->receive_context, keyin);
 
	if ((r = ssh_packet_set_postauth(ssh)) != 0)
 		return r;
 
 	sshbuf_reset(state->input);
	sshbuf_reset(state->output);
	if ((r = sshbuf_get_string_direct(m, &input, &ilen)) != 0 ||
	    (r = sshbuf_get_string_direct(m, &output, &olen)) != 0 ||
	    (r = sshbuf_put(state->input, input, ilen)) != 0 ||
	    (r = sshbuf_put(state->output, output, olen)) != 0)
		return r;

	if (sshbuf_len(m))
		return SSH_ERR_INVALID_FORMAT;
	debug3("%s: done", __func__);
	return 0;
}
