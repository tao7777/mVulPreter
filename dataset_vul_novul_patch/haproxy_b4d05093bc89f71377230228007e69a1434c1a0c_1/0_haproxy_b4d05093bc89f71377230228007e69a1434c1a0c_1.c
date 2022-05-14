static void si_conn_send(struct connection *conn)
{
	struct stream_interface *si = conn->owner;
	struct channel *chn = si->ob;
	int ret;

        if (chn->pipe && conn->xprt->snd_pipe) {
                ret = conn->xprt->snd_pipe(conn, chn->pipe);
                if (ret > 0)
                       chn->flags |= CF_WRITE_PARTIAL | CF_WROTE_DATA;
 
                if (!chn->pipe->data) {
                        put_pipe(chn->pipe);
			chn->pipe = NULL;
		}

		if (conn->flags & CO_FL_ERROR)
			return;
	}

	/* At this point, the pipe is empty, but we may still have data pending
	 * in the normal buffer.
	 */
	if (!chn->buf->o)
		return;

	/* when we're here, we already know that there is no spliced
	 * data left, and that there are sendable buffered data.
	 */
	if (!(conn->flags & (CO_FL_ERROR | CO_FL_SOCK_WR_SH | CO_FL_DATA_WR_SH | CO_FL_WAIT_DATA | CO_FL_HANDSHAKE))) {
		/* check if we want to inform the kernel that we're interested in
		 * sending more data after this call. We want this if :
		 *  - we're about to close after this last send and want to merge
		 *    the ongoing FIN with the last segment.
		 *  - we know we can't send everything at once and must get back
		 *    here because of unaligned data
		 *  - there is still a finite amount of data to forward
		 * The test is arranged so that the most common case does only 2
		 * tests.
		 */
		unsigned int send_flag = 0;

		if ((!(chn->flags & (CF_NEVER_WAIT|CF_SEND_DONTWAIT)) &&
		     ((chn->to_forward && chn->to_forward != CHN_INFINITE_FORWARD) ||
		      (chn->flags & CF_EXPECT_MORE))) ||
		    ((chn->flags & (CF_SHUTW|CF_SHUTW_NOW)) == CF_SHUTW_NOW))
			send_flag |= CO_SFL_MSG_MORE;

		if (chn->flags & CF_STREAMER)
			send_flag |= CO_SFL_STREAMER;
 
                ret = conn->xprt->snd_buf(conn, chn->buf, send_flag);
                if (ret > 0) {
                       chn->flags |= CF_WRITE_PARTIAL | CF_WROTE_DATA;
 
                        if (!chn->buf->o) {
                                /* Always clear both flags once everything has been sent, they're one-shot */
				chn->flags &= ~(CF_EXPECT_MORE | CF_SEND_DONTWAIT);
			}

			/* if some data remain in the buffer, it's only because the
			 * system buffers are full, we will try next time.
			 */
		}
	}

	return;
}
