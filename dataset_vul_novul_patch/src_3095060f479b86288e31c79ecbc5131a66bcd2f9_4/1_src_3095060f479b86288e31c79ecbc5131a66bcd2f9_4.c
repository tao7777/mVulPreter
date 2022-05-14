ssh_packet_get_compress_state(struct sshbuf *m, struct ssh *ssh)
{
	struct session_state *state = ssh->state;
	struct sshbuf *b;
	int r;
	if ((b = sshbuf_new()) == NULL)
		return SSH_ERR_ALLOC_FAIL;
	if (state->compression_in_started) {
		if ((r = sshbuf_put_string(b, &state->compression_in_stream,
		    sizeof(state->compression_in_stream))) != 0)
			goto out;
	} else if ((r = sshbuf_put_string(b, NULL, 0)) != 0)
		goto out;
	if (state->compression_out_started) {
		if ((r = sshbuf_put_string(b, &state->compression_out_stream,
		    sizeof(state->compression_out_stream))) != 0)
			goto out;
	} else if ((r = sshbuf_put_string(b, NULL, 0)) != 0)
		goto out;
	r = sshbuf_put_stringb(m, b);
 out:
	sshbuf_free(b);
	return r;
}
