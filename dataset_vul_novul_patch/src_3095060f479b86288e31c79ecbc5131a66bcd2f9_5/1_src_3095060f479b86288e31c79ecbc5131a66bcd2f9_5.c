ssh_packet_get_state(struct ssh *ssh, struct sshbuf *m)
{
	struct session_state *state = ssh->state;
	u_char *p;
	size_t slen, rlen;
	int r, ssh1cipher;

	if (!compat20) {
		ssh1cipher = cipher_ctx_get_number(state->receive_context);
		slen = cipher_get_keyiv_len(state->send_context);
		rlen = cipher_get_keyiv_len(state->receive_context);
		if ((r = sshbuf_put_u32(m, state->remote_protocol_flags)) != 0 ||
		    (r = sshbuf_put_u32(m, ssh1cipher)) != 0 ||
		    (r = sshbuf_put_string(m, state->ssh1_key, state->ssh1_keylen)) != 0 ||
		    (r = sshbuf_put_u32(m, slen)) != 0 ||
		    (r = sshbuf_reserve(m, slen, &p)) != 0 ||
		    (r = cipher_get_keyiv(state->send_context, p, slen)) != 0 ||
		    (r = sshbuf_put_u32(m, rlen)) != 0 ||
		    (r = sshbuf_reserve(m, rlen, &p)) != 0 ||
		    (r = cipher_get_keyiv(state->receive_context, p, rlen)) != 0)
			return r;
	} else {
		if ((r = kex_to_blob(m, ssh->kex)) != 0 ||
		    (r = newkeys_to_blob(m, ssh, MODE_OUT)) != 0 ||
		    (r = newkeys_to_blob(m, ssh, MODE_IN)) != 0 ||
		    (r = sshbuf_put_u64(m, state->rekey_limit)) != 0 ||
		    (r = sshbuf_put_u32(m, state->rekey_interval)) != 0 ||
		    (r = sshbuf_put_u32(m, state->p_send.seqnr)) != 0 ||
		    (r = sshbuf_put_u64(m, state->p_send.blocks)) != 0 ||
		    (r = sshbuf_put_u32(m, state->p_send.packets)) != 0 ||
		    (r = sshbuf_put_u64(m, state->p_send.bytes)) != 0 ||
		    (r = sshbuf_put_u32(m, state->p_read.seqnr)) != 0 ||
		    (r = sshbuf_put_u64(m, state->p_read.blocks)) != 0 ||
		    (r = sshbuf_put_u32(m, state->p_read.packets)) != 0 ||
		    (r = sshbuf_put_u64(m, state->p_read.bytes)) != 0)
			return r;
	}

	slen = cipher_get_keycontext(state->send_context, NULL);
	rlen = cipher_get_keycontext(state->receive_context, NULL);
	if ((r = sshbuf_put_u32(m, slen)) != 0 ||
	    (r = sshbuf_reserve(m, slen, &p)) != 0)
		return r;
	if (cipher_get_keycontext(state->send_context, p) != (int)slen)
		return SSH_ERR_INTERNAL_ERROR;
	if ((r = sshbuf_put_u32(m, rlen)) != 0 ||
	    (r = sshbuf_reserve(m, rlen, &p)) != 0)
 		return r;
 	if (cipher_get_keycontext(state->receive_context, p) != (int)rlen)
 		return SSH_ERR_INTERNAL_ERROR;
	if ((r = ssh_packet_get_compress_state(m, ssh)) != 0 ||
	    (r = sshbuf_put_stringb(m, state->input)) != 0 ||
 	    (r = sshbuf_put_stringb(m, state->output)) != 0)
 		return r;
 
	return 0;
}
