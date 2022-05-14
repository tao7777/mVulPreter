mux_session_confirm(int id, int success, void *arg)
{
	struct mux_session_confirm_ctx *cctx = arg;
	const char *display;
	Channel *c, *cc;
	int i;
	Buffer reply;

	if (cctx == NULL)
		fatal("%s: cctx == NULL", __func__);
	if ((c = channel_by_id(id)) == NULL)
		fatal("%s: no channel for id %d", __func__, id);
	if ((cc = channel_by_id(c->ctl_chan)) == NULL)
		fatal("%s: channel %d lacks control channel %d", __func__,
		    id, c->ctl_chan);

	if (!success) {
		debug3("%s: sending failure reply", __func__);
		/* prepare reply */
		buffer_init(&reply);
		buffer_put_int(&reply, MUX_S_FAILURE);
		buffer_put_int(&reply, cctx->rid);
		buffer_put_cstring(&reply, "Session open refused by peer");
		goto done;
	}

	display = getenv("DISPLAY");
	if (cctx->want_x_fwd && options.forward_x11 && display != NULL) {
 		char *proto, *data;
 
 		/* Get reasonable local authentication information. */
		client_x11_get_proto(display, options.xauth_location,
 		    options.forward_x11_trusted, options.forward_x11_timeout,
		    &proto, &data);
		/* Request forwarding with authentication spoofing. */
		debug("Requesting X11 forwarding with authentication "
		    "spoofing.");
		x11_request_forwarding_with_spoofing(id, display, proto,
		    data, 1);
		client_expect_confirm(id, "X11 forwarding", CONFIRM_WARN);
		/* XXX exit_on_forward_failure */
 	}
 
 	if (cctx->want_agent_fwd && options.forward_agent) {
		packet_send();
	}

	client_session2_setup(id, cctx->want_tty, cctx->want_subsys,
	    cctx->term, &cctx->tio, c->rfd, &cctx->cmd, cctx->env);

	debug3("%s: sending success reply", __func__);
	/* prepare reply */
	buffer_init(&reply);
	buffer_put_int(&reply, MUX_S_SESSION_OPENED);
	buffer_put_int(&reply, cctx->rid);
	buffer_put_int(&reply, c->self);

 done:
	/* Send reply */
	buffer_put_string(&cc->output, buffer_ptr(&reply), buffer_len(&reply));
	buffer_free(&reply);

	if (cc->mux_pause <= 0)
		fatal("%s: mux_pause %d", __func__, cc->mux_pause);
	cc->mux_pause = 0; /* start processing messages again */
	c->open_confirm_ctx = NULL;
	buffer_free(&cctx->cmd);
	free(cctx->term);
	if (cctx->env != NULL) {
		for (i = 0; cctx->env[i] != NULL; i++)
			free(cctx->env[i]);
		free(cctx->env);
	}
	free(cctx);
}
