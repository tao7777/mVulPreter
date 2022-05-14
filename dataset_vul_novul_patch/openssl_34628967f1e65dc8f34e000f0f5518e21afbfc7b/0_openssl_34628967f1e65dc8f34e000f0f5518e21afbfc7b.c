 dtls1_hm_fragment_free(hm_fragment *frag)
        {

       if (frag->msg_header.is_ccs)
               {
               EVP_CIPHER_CTX_free(frag->msg_header.saved_retransmit_state.enc_write_ctx);
               EVP_MD_CTX_destroy(frag->msg_header.saved_retransmit_state.write_hash);
               }
        if (frag->fragment) OPENSSL_free(frag->fragment);
        if (frag->reassembly) OPENSSL_free(frag->reassembly);
        OPENSSL_free(frag);
	int curr_mtu;
	unsigned int len, frag_off, mac_size, blocksize;

	/* AHA!  Figure out the MTU, and stick to the right size */
	if (s->d1->mtu < dtls1_min_mtu() && !(SSL_get_options(s) & SSL_OP_NO_QUERY_MTU))
		{
		s->d1->mtu = 
			BIO_ctrl(SSL_get_wbio(s), BIO_CTRL_DGRAM_QUERY_MTU, 0, NULL);

		/* I've seen the kernel return bogus numbers when it doesn't know
		 * (initial write), so just make sure we have a reasonable number */
		if (s->d1->mtu < dtls1_min_mtu())
			{
			s->d1->mtu = 0;
			s->d1->mtu = dtls1_guess_mtu(s->d1->mtu);
			BIO_ctrl(SSL_get_wbio(s), BIO_CTRL_DGRAM_SET_MTU, 
				s->d1->mtu, NULL);
			}
		}
#if 0 
	mtu = s->d1->mtu;

	fprintf(stderr, "using MTU = %d\n", mtu);

	mtu -= (DTLS1_HM_HEADER_LENGTH + DTLS1_RT_HEADER_LENGTH);

	curr_mtu = mtu - BIO_wpending(SSL_get_wbio(s));

	if ( curr_mtu > 0)
		mtu = curr_mtu;
	else if ( ( ret = BIO_flush(SSL_get_wbio(s))) <= 0)
		return ret;

	if ( BIO_wpending(SSL_get_wbio(s)) + s->init_num >= mtu)
		{
		ret = BIO_flush(SSL_get_wbio(s));
		if ( ret <= 0)
			return ret;
		mtu = s->d1->mtu - (DTLS1_HM_HEADER_LENGTH + DTLS1_RT_HEADER_LENGTH);
		}
#endif

	OPENSSL_assert(s->d1->mtu >= dtls1_min_mtu());  /* should have something reasonable now */

	if ( s->init_off == 0  && type == SSL3_RT_HANDSHAKE)
		OPENSSL_assert(s->init_num == 
			(int)s->d1->w_msg_hdr.msg_len + DTLS1_HM_HEADER_LENGTH);

	if (s->write_hash)
		mac_size = EVP_MD_CTX_size(s->write_hash);
	else
		mac_size = 0;

	if (s->enc_write_ctx && 
		(EVP_CIPHER_mode( s->enc_write_ctx->cipher) & EVP_CIPH_CBC_MODE))
		blocksize = 2 * EVP_CIPHER_block_size(s->enc_write_ctx->cipher);
	else
		blocksize = 0;

	frag_off = 0;
	while( s->init_num)
		{
		curr_mtu = s->d1->mtu - BIO_wpending(SSL_get_wbio(s)) - 
			DTLS1_RT_HEADER_LENGTH - mac_size - blocksize;

		if ( curr_mtu <= DTLS1_HM_HEADER_LENGTH)
			{
			/* grr.. we could get an error if MTU picked was wrong */
			ret = BIO_flush(SSL_get_wbio(s));
			if ( ret <= 0)
				return ret;
			curr_mtu = s->d1->mtu - DTLS1_RT_HEADER_LENGTH -
				mac_size - blocksize;
			}

		if ( s->init_num > curr_mtu)
			len = curr_mtu;
		else
			len = s->init_num;


		/* XDTLS: this function is too long.  split out the CCS part */
		if ( type == SSL3_RT_HANDSHAKE)
			{
			if ( s->init_off != 0)
				{
				OPENSSL_assert(s->init_off > DTLS1_HM_HEADER_LENGTH);
				s->init_off -= DTLS1_HM_HEADER_LENGTH;
				s->init_num += DTLS1_HM_HEADER_LENGTH;

				if ( s->init_num > curr_mtu)
					len = curr_mtu;
				else
					len = s->init_num;
				}

			dtls1_fix_message_header(s, frag_off, 
				len - DTLS1_HM_HEADER_LENGTH);

			dtls1_write_message_header(s, (unsigned char *)&s->init_buf->data[s->init_off]);

			OPENSSL_assert(len >= DTLS1_HM_HEADER_LENGTH);
			}

		ret=dtls1_write_bytes(s,type,&s->init_buf->data[s->init_off],
			len);
		if (ret < 0)
			{
			/* might need to update MTU here, but we don't know
			 * which previous packet caused the failure -- so can't
			 * really retransmit anything.  continue as if everything
			 * is fine and wait for an alert to handle the
			 * retransmit 
			 */
			if ( BIO_ctrl(SSL_get_wbio(s),
				BIO_CTRL_DGRAM_MTU_EXCEEDED, 0, NULL) > 0 )
				s->d1->mtu = BIO_ctrl(SSL_get_wbio(s),
					BIO_CTRL_DGRAM_QUERY_MTU, 0, NULL);
			else
				return(-1);
			}
		else
			{

			/* bad if this assert fails, only part of the handshake
			 * message got sent.  but why would this happen? */
			OPENSSL_assert(len == (unsigned int)ret);

			if (type == SSL3_RT_HANDSHAKE && ! s->d1->retransmitting)
				{
				/* should not be done for 'Hello Request's, but in that case
				 * we'll ignore the result anyway */
				unsigned char *p = (unsigned char *)&s->init_buf->data[s->init_off];
				const struct hm_header_st *msg_hdr = &s->d1->w_msg_hdr;
				int xlen;

				if (frag_off == 0 && s->version != DTLS1_BAD_VER)
					{
					/* reconstruct message header is if it
					 * is being sent in single fragment */
					*p++ = msg_hdr->type;
					l2n3(msg_hdr->msg_len,p);
					s2n (msg_hdr->seq,p);
					l2n3(0,p);
					l2n3(msg_hdr->msg_len,p);
					p  -= DTLS1_HM_HEADER_LENGTH;
					xlen = ret;
					}
				else
					{
					p  += DTLS1_HM_HEADER_LENGTH;
					xlen = ret - DTLS1_HM_HEADER_LENGTH;
					}

				ssl3_finish_mac(s, p, xlen);
				}

			if (ret == s->init_num)
				{
				if (s->msg_callback)
					s->msg_callback(1, s->version, type, s->init_buf->data, 
						(size_t)(s->init_off + s->init_num), s, 
						s->msg_callback_arg);

				s->init_off = 0;  /* done writing this message */
				s->init_num = 0;

				return(1);
				}
			s->init_off+=ret;
			s->init_num-=ret;
			frag_off += (ret -= DTLS1_HM_HEADER_LENGTH);
			}
		}
	return(0);
	}
