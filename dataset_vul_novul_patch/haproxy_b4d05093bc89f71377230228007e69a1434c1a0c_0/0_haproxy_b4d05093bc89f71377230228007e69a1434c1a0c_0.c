int http_request_forward_body(struct session *s, struct channel *req, int an_bit)
{
	struct http_txn *txn = &s->txn;
	struct http_msg *msg = &s->txn.req;

	if (unlikely(msg->msg_state < HTTP_MSG_BODY))
		return 0;

	if ((req->flags & (CF_READ_ERROR|CF_READ_TIMEOUT|CF_WRITE_ERROR|CF_WRITE_TIMEOUT)) ||
	    ((req->flags & CF_SHUTW) && (req->to_forward || req->buf->o))) {
		/* Output closed while we were sending data. We must abort and
		 * wake the other side up.
		 */
		msg->msg_state = HTTP_MSG_ERROR;
		http_resync_states(s);
		return 1;
	}

	/* Note that we don't have to send 100-continue back because we don't
	 * need the data to complete our job, and it's up to the server to
	 * decide whether to return 100, 417 or anything else in return of
	 * an "Expect: 100-continue" header.
	 */

	if (msg->sov > 0) {
		/* we have msg->sov which points to the first byte of message
		 * body, and req->buf.p still points to the beginning of the
		 * message. We forward the headers now, as we don't need them
		 * anymore, and we want to flush them.
		 */
		b_adv(req->buf, msg->sov);
		msg->next -= msg->sov;
		msg->sov = 0;

		/* The previous analysers guarantee that the state is somewhere
		 * between MSG_BODY and the first MSG_DATA. So msg->sol and
		 * msg->next are always correct.
		 */
		if (msg->msg_state < HTTP_MSG_CHUNK_SIZE) {
			if (msg->flags & HTTP_MSGF_TE_CHNK)
				msg->msg_state = HTTP_MSG_CHUNK_SIZE;
			else
				msg->msg_state = HTTP_MSG_DATA;
		}
	}

	/* Some post-connect processing might want us to refrain from starting to
	 * forward data. Currently, the only reason for this is "balance url_param"
	 * whichs need to parse/process the request after we've enabled forwarding.
	 */
	if (unlikely(msg->flags & HTTP_MSGF_WAIT_CONN)) {
		if (!(s->rep->flags & CF_READ_ATTACHED)) {
			channel_auto_connect(req);
			req->flags |= CF_WAKE_CONNECT;
			goto missing_data;
		}
		msg->flags &= ~HTTP_MSGF_WAIT_CONN;
	}

	/* in most states, we should abort in case of early close */
	channel_auto_close(req);

	if (req->to_forward) {
		/* We can't process the buffer's contents yet */
		req->flags |= CF_WAKE_WRITE;
		goto missing_data;
	}

	while (1) {
		if (msg->msg_state == HTTP_MSG_DATA) {
			/* must still forward */
			/* we may have some pending data starting at req->buf->p */
			if (msg->chunk_len > req->buf->i - msg->next) {
				req->flags |= CF_WAKE_WRITE;
				goto missing_data;
			}
			msg->next += msg->chunk_len;
			msg->chunk_len = 0;

			/* nothing left to forward */
			if (msg->flags & HTTP_MSGF_TE_CHNK)
				msg->msg_state = HTTP_MSG_CHUNK_CRLF;
			else
				msg->msg_state = HTTP_MSG_DONE;
		}
		else if (msg->msg_state == HTTP_MSG_CHUNK_SIZE) {
			/* read the chunk size and assign it to ->chunk_len, then
			 * set ->next to point to the body and switch to DATA or
			 * TRAILERS state.
			 */
			int ret = http_parse_chunk_size(msg);

			if (ret == 0)
				goto missing_data;
			else if (ret < 0) {
				session_inc_http_err_ctr(s);
				if (msg->err_pos >= 0)
					http_capture_bad_message(&s->fe->invalid_req, s, msg, HTTP_MSG_CHUNK_SIZE, s->be);
				goto return_bad_req;
			}
			/* otherwise we're in HTTP_MSG_DATA or HTTP_MSG_TRAILERS state */
		}
		else if (msg->msg_state == HTTP_MSG_CHUNK_CRLF) {
			/* we want the CRLF after the data */
			int ret = http_skip_chunk_crlf(msg);

			if (ret == 0)
				goto missing_data;
			else if (ret < 0) {
				session_inc_http_err_ctr(s);
				if (msg->err_pos >= 0)
					http_capture_bad_message(&s->fe->invalid_req, s, msg, HTTP_MSG_CHUNK_CRLF, s->be);
				goto return_bad_req;
			}
			/* we're in MSG_CHUNK_SIZE now */
		}
		else if (msg->msg_state == HTTP_MSG_TRAILERS) {
			int ret = http_forward_trailers(msg);

			if (ret == 0)
				goto missing_data;
			else if (ret < 0) {
				session_inc_http_err_ctr(s);
				if (msg->err_pos >= 0)
					http_capture_bad_message(&s->fe->invalid_req, s, msg, HTTP_MSG_TRAILERS, s->be);
				goto return_bad_req;
			}
			/* we're in HTTP_MSG_DONE now */
		}
		else {
			int old_state = msg->msg_state;

			/* other states, DONE...TUNNEL */

			/* we may have some pending data starting at req->buf->p
                         * such as last chunk of data or trailers.
                         */
                        b_adv(req->buf, msg->next);
                       if (unlikely(!(s->req->flags & CF_WROTE_DATA)))
                                msg->sov -= msg->next;
                        msg->next = 0;
 
			/* for keep-alive we don't want to forward closes on DONE */
			if ((txn->flags & TX_CON_WANT_MSK) == TX_CON_WANT_KAL ||
			    (txn->flags & TX_CON_WANT_MSK) == TX_CON_WANT_SCL)
				channel_dont_close(req);
			if (http_resync_states(s)) {
				/* some state changes occurred, maybe the analyser
				 * was disabled too.
				 */
				if (unlikely(msg->msg_state == HTTP_MSG_ERROR)) {
					if (req->flags & CF_SHUTW) {
						/* request errors are most likely due to
						 * the server aborting the transfer.
						 */
						goto aborted_xfer;
					}
					if (msg->err_pos >= 0)
						http_capture_bad_message(&s->fe->invalid_req, s, msg, old_state, s->be);
					goto return_bad_req;
				}
				return 1;
			}

			/* If "option abortonclose" is set on the backend, we
			 * want to monitor the client's connection and forward
			 * any shutdown notification to the server, which will
			 * decide whether to close or to go on processing the
			 * request.
			 */
			if (s->be->options & PR_O_ABRT_CLOSE) {
				channel_auto_read(req);
				channel_auto_close(req);
			}
			else if (s->txn.meth == HTTP_METH_POST) {
				/* POST requests may require to read extra CRLF
				 * sent by broken browsers and which could cause
				 * an RST to be sent upon close on some systems
				 * (eg: Linux).
				 */
				channel_auto_read(req);
			}

			return 0;
		}
	}

  missing_data:
        /* we may have some pending data starting at req->buf->p */
        b_adv(req->buf, msg->next);
       if (unlikely(!(s->req->flags & CF_WROTE_DATA)))
                msg->sov -= msg->next + MIN(msg->chunk_len, req->buf->i);
 
        msg->next = 0;
	msg->chunk_len -= channel_forward(req, msg->chunk_len);

	/* stop waiting for data if the input is closed before the end */
	if (req->flags & CF_SHUTR) {
		if (!(s->flags & SN_ERR_MASK))
			s->flags |= SN_ERR_CLICL;
		if (!(s->flags & SN_FINST_MASK)) {
			if (txn->rsp.msg_state < HTTP_MSG_ERROR)
				s->flags |= SN_FINST_H;
			else
				s->flags |= SN_FINST_D;
		}

		s->fe->fe_counters.cli_aborts++;
		s->be->be_counters.cli_aborts++;
		if (objt_server(s->target))
			objt_server(s->target)->counters.cli_aborts++;

		goto return_bad_req_stats_ok;
	}

	/* waiting for the last bits to leave the buffer */
	if (req->flags & CF_SHUTW)
		goto aborted_xfer;

	/* When TE: chunked is used, we need to get there again to parse remaining
	 * chunks even if the client has closed, so we don't want to set CF_DONTCLOSE.
	 */
	if (msg->flags & HTTP_MSGF_TE_CHNK)
		channel_dont_close(req);

	/* We know that more data are expected, but we couldn't send more that
	 * what we did. So we always set the CF_EXPECT_MORE flag so that the
	 * system knows it must not set a PUSH on this first part. Interactive
	 * modes are already handled by the stream sock layer. We must not do
	 * this in content-length mode because it could present the MSG_MORE
	 * flag with the last block of forwarded data, which would cause an
	 * additional delay to be observed by the receiver.
	 */
	if (msg->flags & HTTP_MSGF_TE_CHNK)
		req->flags |= CF_EXPECT_MORE;

	return 0;

 return_bad_req: /* let's centralize all bad requests */
	s->fe->fe_counters.failed_req++;
	if (s->listener->counters)
		s->listener->counters->failed_req++;

 return_bad_req_stats_ok:
	/* we may have some pending data starting at req->buf->p */
	b_adv(req->buf, msg->next);
	msg->next = 0;

	txn->req.msg_state = HTTP_MSG_ERROR;
	if (txn->status) {
		/* Note: we don't send any error if some data were already sent */
		stream_int_retnclose(req->prod, NULL);
	} else {
		txn->status = 400;
		stream_int_retnclose(req->prod, http_error_message(s, HTTP_ERR_400));
	}
	req->analysers = 0;
	s->rep->analysers = 0; /* we're in data phase, we want to abort both directions */

	if (!(s->flags & SN_ERR_MASK))
		s->flags |= SN_ERR_PRXCOND;
	if (!(s->flags & SN_FINST_MASK)) {
		if (txn->rsp.msg_state < HTTP_MSG_ERROR)
			s->flags |= SN_FINST_H;
		else
			s->flags |= SN_FINST_D;
	}
	return 0;

 aborted_xfer:
	txn->req.msg_state = HTTP_MSG_ERROR;
	if (txn->status) {
		/* Note: we don't send any error if some data were already sent */
		stream_int_retnclose(req->prod, NULL);
	} else {
		txn->status = 502;
		stream_int_retnclose(req->prod, http_error_message(s, HTTP_ERR_502));
	}
	req->analysers = 0;
	s->rep->analysers = 0; /* we're in data phase, we want to abort both directions */

	s->fe->fe_counters.srv_aborts++;
	s->be->be_counters.srv_aborts++;
	if (objt_server(s->target))
		objt_server(s->target)->counters.srv_aborts++;

	if (!(s->flags & SN_ERR_MASK))
		s->flags |= SN_ERR_SRVCL;
	if (!(s->flags & SN_FINST_MASK)) {
		if (txn->rsp.msg_state < HTTP_MSG_ERROR)
			s->flags |= SN_FINST_H;
		else
			s->flags |= SN_FINST_D;
	}
	return 0;
}
