ssh_packet_set_compress_state(struct ssh *ssh, struct sshbuf *m)
{
	struct session_state *state = ssh->state;
	struct sshbuf *b = NULL;
	int r;
	const u_char *inblob, *outblob;
	size_t inl, outl;
	if ((r = sshbuf_froms(m, &b)) != 0)
		goto out;
	if ((r = sshbuf_get_string_direct(b, &inblob, &inl)) != 0 ||
	    (r = sshbuf_get_string_direct(b, &outblob, &outl)) != 0)
		goto out;
	if (inl == 0)
		state->compression_in_started = 0;
	else if (inl != sizeof(state->compression_in_stream)) {
		r = SSH_ERR_INTERNAL_ERROR;
		goto out;
	} else {
		state->compression_in_started = 1;
		memcpy(&state->compression_in_stream, inblob, inl);
	}
	if (outl == 0)
		state->compression_out_started = 0;
	else if (outl != sizeof(state->compression_out_stream)) {
		r = SSH_ERR_INTERNAL_ERROR;
		goto out;
	} else {
		state->compression_out_started = 1;
		memcpy(&state->compression_out_stream, outblob, outl);
	}
	r = 0;
 out:
	sshbuf_free(b);
	return r;
}
