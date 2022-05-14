void chain_reply(struct smb_request *req)
{
	size_t smblen = smb_len(req->inbuf);
	size_t already_used, length_needed;
	uint8_t chain_cmd;
	uint32_t chain_offset;	/* uint32_t to avoid overflow */

	uint8_t wct;
	uint16_t *vwv;
	uint16_t buflen;
	uint8_t *buf;

	if (IVAL(req->outbuf, smb_rcls) != 0) {
		fixup_chain_error_packet(req);
	}

	/*
	 * Any of the AndX requests and replies have at least a wct of
	 * 2. vwv[0] is the next command, vwv[1] is the offset from the
	 * beginning of the SMB header to the next wct field.
	 *
	 * None of the AndX requests put anything valuable in vwv[0] and [1],
	 * so we can overwrite it here to form the chain.
         */
 
        if ((req->wct < 2) || (CVAL(req->outbuf, smb_wct) < 2)) {
               if (req->chain_outbuf == NULL) {
                       req->chain_outbuf = TALLOC_REALLOC_ARRAY(
                               req, req->outbuf, uint8_t,
                               smb_len(req->outbuf) + 4);
                       if (req->chain_outbuf == NULL) {
                               smb_panic("talloc failed");
                       }
               }
               req->outbuf = NULL;
                goto error;
        }
 

	if (req->chain_outbuf == NULL) {
		/*
		 * In req->chain_outbuf we collect all the replies. Start the
		 * chain by copying in the first reply.
		 *
		 * We do the realloc because later on we depend on
		 * talloc_get_size to determine the length of
		 * chain_outbuf. The reply_xxx routines might have
		 * over-allocated (reply_pipe_read_and_X used to be such an
		 * example).
		 */
		req->chain_outbuf = TALLOC_REALLOC_ARRAY(
			req, req->outbuf, uint8_t, smb_len(req->outbuf) + 4);
		if (req->chain_outbuf == NULL) {
			goto error;
		}
		req->outbuf = NULL;
	} else {
		/*
		 * Update smb headers where subsequent chained commands
                req->chain_outbuf = TALLOC_REALLOC_ARRAY(
                        req, req->outbuf, uint8_t, smb_len(req->outbuf) + 4);
                if (req->chain_outbuf == NULL) {
                       smb_panic("talloc failed");
                }
                req->outbuf = NULL;
        } else {
				      CVAL(req->outbuf, smb_wct),
				      (uint16_t *)(req->outbuf + smb_vwv),
				      0, smb_buflen(req->outbuf),
				      (uint8_t *)smb_buf(req->outbuf))) {
			goto error;
		}
		TALLOC_FREE(req->outbuf);
	}

	/*
	 * We use the old request's vwv field to grab the next chained command
	 * and offset into the chained fields.
	 */

	chain_cmd = CVAL(req->vwv+0, 0);
	chain_offset = SVAL(req->vwv+1, 0);

	if (chain_cmd == 0xff) {
		/*
		 * End of chain, no more requests from the client. So ship the
		 * replies.
		 */
		smb_setlen((char *)(req->chain_outbuf),
			   talloc_get_size(req->chain_outbuf) - 4);

		if (!srv_send_smb(smbd_server_fd(), (char *)req->chain_outbuf,
				  true, req->seqnum+1,
				  IS_CONN_ENCRYPTED(req->conn)
				  ||req->encrypted,
				  &req->pcd)) {
			exit_server_cleanly("chain_reply: srv_send_smb "
					    "failed.");
		}
		TALLOC_FREE(req->chain_outbuf);
		req->done = true;
		return;
	}

	/* add a new perfcounter for this element of chain */
	SMB_PERFCOUNT_ADD(&req->pcd);
	SMB_PERFCOUNT_SET_OP(&req->pcd, chain_cmd);
	SMB_PERFCOUNT_SET_MSGLEN_IN(&req->pcd, smblen);

	/*
	 * Check if the client tries to fool us. The request so far uses the
	 * space to the end of the byte buffer in the request just
	 * processed. The chain_offset can't point into that area. If that was
	 * the case, we could end up with an endless processing of the chain,
	 * we would always handle the same request.
	 */

	already_used = PTR_DIFF(req->buf+req->buflen, smb_base(req->inbuf));
	if (chain_offset < already_used) {
		goto error;
	}

	/*
	 * Next check: Make sure the chain offset does not point beyond the
	 * overall smb request length.
	 */

	length_needed = chain_offset+1;	/* wct */
	if (length_needed > smblen) {
		goto error;
	}

	/*
	 * Now comes the pointer magic. Goal here is to set up req->vwv and
	 * req->buf correctly again to be able to call the subsequent
	 * switch_message(). The chain offset (the former vwv[1]) points at
	 * the new wct field.
	 */

	wct = CVAL(smb_base(req->inbuf), chain_offset);

	/*
	 * Next consistency check: Make the new vwv array fits in the overall
	 * smb request.
	 */

	length_needed += (wct+1)*sizeof(uint16_t); /* vwv+buflen */
	if (length_needed > smblen) {
		goto error;
	}
	vwv = (uint16_t *)(smb_base(req->inbuf) + chain_offset + 1);

	/*
	 * Now grab the new byte buffer....
	 */

	buflen = SVAL(vwv+wct, 0);

	/*
	 * .. and check that it fits.
	 */

	length_needed += buflen;
	if (length_needed > smblen) {
		goto error;
	}
	buf = (uint8_t *)(vwv+wct+1);

	req->cmd = chain_cmd;
	req->wct = wct;
	req->vwv = vwv;
	req->buflen = buflen;
	req->buf = buf;

	switch_message(chain_cmd, req, smblen);

	if (req->outbuf == NULL) {
		/*
		 * This happens if the chained command has suspended itself or
		 * if it has called srv_send_smb() itself.
		 */
		return;
	}

	/*
	 * We end up here if the chained command was not itself chained or
	 * suspended, but for example a close() command. We now need to splice
	 * the chained commands' outbuf into the already built up chain_outbuf
	 * and ship the result.
	 */
	goto done;

 error:
	/*
	 * We end up here if there's any error in the chain syntax. Report a
	 * DOS error, just like Windows does.
	 */
	reply_force_doserror(req, ERRSRV, ERRerror);
	fixup_chain_error_packet(req);

 done:
	/*
	 * This scary statement intends to set the
	 * FLAGS2_32_BIT_ERROR_CODES flg2 field in req->chain_outbuf
	 * to the value req->outbuf carries
	 */
	SSVAL(req->chain_outbuf, smb_flg2,
	      (SVAL(req->chain_outbuf, smb_flg2) & ~FLAGS2_32_BIT_ERROR_CODES)
	      | (SVAL(req->outbuf, smb_flg2) & FLAGS2_32_BIT_ERROR_CODES));

	/*
	 * Transfer the error codes from the subrequest to the main one
	 */
	SSVAL(req->chain_outbuf, smb_rcls, SVAL(req->outbuf, smb_rcls));
	SSVAL(req->chain_outbuf, smb_err, SVAL(req->outbuf, smb_err));

	if (!smb_splice_chain(&req->chain_outbuf,
			      CVAL(req->outbuf, smb_com),
			      CVAL(req->outbuf, smb_wct),
			      (uint16_t *)(req->outbuf + smb_vwv),
			      0, smb_buflen(req->outbuf),
			      (uint8_t *)smb_buf(req->outbuf))) {
		exit_server_cleanly("chain_reply: smb_splice_chain failed\n");
	}
	TALLOC_FREE(req->outbuf);

	smb_setlen((char *)(req->chain_outbuf),
		   talloc_get_size(req->chain_outbuf) - 4);

	show_msg((char *)(req->chain_outbuf));

	if (!srv_send_smb(smbd_server_fd(), (char *)req->chain_outbuf,
			  true, req->seqnum+1,
			  IS_CONN_ENCRYPTED(req->conn)||req->encrypted,
			  &req->pcd)) {
		exit_server_cleanly("construct_reply: srv_send_smb failed.");
	}
	TALLOC_FREE(req->chain_outbuf);
	req->done = true;
}
