static int piv_general_io(sc_card_t *card, int ins, int p1, int p2,
	const u8 * sendbuf, size_t sendbuflen, u8 ** recvbuf,
	size_t * recvbuflen)
{
	int r;
	sc_apdu_t apdu;
	u8 rbufinitbuf[4096];
	u8 *rbuf;
	size_t rbuflen;
	unsigned int cla_out, tag_out;
	const u8 *body;
	size_t bodylen;
	int find_len = 0;
	piv_private_data_t * priv = PIV_DATA(card);


	SC_FUNC_CALLED(card->ctx, SC_LOG_DEBUG_VERBOSE);

	sc_log(card->ctx,
	       "%02x %02x %02x %"SC_FORMAT_LEN_SIZE_T"u : %"SC_FORMAT_LEN_SIZE_T"u %"SC_FORMAT_LEN_SIZE_T"u",
	       ins, p1, p2, sendbuflen, card->max_send_size,
	       card->max_recv_size);

	rbuf = rbufinitbuf;
	rbuflen = sizeof(rbufinitbuf);

	/* if caller provided a buffer and length */
	if (recvbuf && *recvbuf && recvbuflen && *recvbuflen) {
		rbuf = *recvbuf;
		rbuflen = *recvbuflen;
	}

	r = sc_lock(card);
	if (r != SC_SUCCESS)
		LOG_FUNC_RETURN(card->ctx, r);

	sc_format_apdu(card, &apdu,
			recvbuf ? SC_APDU_CASE_4_SHORT: SC_APDU_CASE_3_SHORT,
			ins, p1, p2);
	apdu.flags |= SC_APDU_FLAGS_CHAINING;
	/* if looking for length of object, dont try and read the rest of buffer here */
	if (rbuflen == 8 && card->reader->active_protocol == SC_PROTO_T1) {
		apdu.flags |= SC_APDU_FLAGS_NO_GET_RESP;
		find_len = 1;
	}

	apdu.lc = sendbuflen;
	apdu.datalen = sendbuflen;
	apdu.data = sendbuf;

	if (recvbuf) {
		apdu.resp = rbuf;
		apdu.le = (rbuflen > 256) ? 256 : rbuflen;
		apdu.resplen = rbuflen;
	} else {
		 apdu.resp =  rbuf;
		 apdu.le = 0;
		 apdu.resplen = 0;
	}

	sc_log(card->ctx,
	       "calling sc_transmit_apdu flags=%lx le=%"SC_FORMAT_LEN_SIZE_T"u, resplen=%"SC_FORMAT_LEN_SIZE_T"u, resp=%p",
	       apdu.flags, apdu.le, apdu.resplen, apdu.resp);

	/* with new adpu.c and chaining, this actually reads the whole object */
	r = sc_transmit_apdu(card, &apdu);

	sc_log(card->ctx,
	       "DEE r=%d apdu.resplen=%"SC_FORMAT_LEN_SIZE_T"u sw1=%02x sw2=%02x",
	       r, apdu.resplen, apdu.sw1, apdu.sw2);
	if (r < 0) {
		sc_log(card->ctx, "Transmit failed");
		goto err;
	}

	if (!(find_len && apdu.sw1 == 0x61))
	    r = sc_check_sw(card, apdu.sw1, apdu.sw2);

/* TODO: - DEE look later at tag vs size read too */
	if (r < 0) {
		sc_log(card->ctx, "Card returned error ");
		goto err;
	}

	/*
	 * See how much we read and make sure it is asn1
	 * if not, return 0 indicating no data found
	 */


	rbuflen = 0;  /* in case rseplen < 3  i.e. not parseable */
	/* we may only be using get data to test the security status of the card, so zero length is OK */
	if ( recvbuflen && recvbuf && apdu.resplen > 3 && priv->pin_cmd_noparse != 1) {
		*recvbuflen = 0;
		/* we should have all the tag data, so we have to tell sc_asn1_find_tag
 		 * the buffer is bigger, so it will not produce "ASN1.tag too long!" */
 
 		body = rbuf;
		if (sc_asn1_read_tag(&body, rbuflen, &cla_out, &tag_out, &bodylen) !=  SC_SUCCESS
 				|| body == NULL)  {
 			/* only early beta cards had this problem */
 			sc_log(card->ctx, "***** received buffer tag MISSING ");
			body = rbuf;
			/* some readers/cards might return 6c 00 */
			if (apdu.sw1 == 0x61  || apdu.sw2 == 0x6c )
				bodylen = 12000;
			else
				bodylen = apdu.resplen;
		}

		rbuflen = body - rbuf + bodylen;

		/* if using internal buffer, alloc new one */
		if (rbuf == rbufinitbuf) {
			*recvbuf = malloc(rbuflen);
			if (*recvbuf == NULL) {
				r = SC_ERROR_OUT_OF_MEMORY;
				goto err;
			}

			memcpy(*recvbuf, rbuf, rbuflen); /* copy tag too */
		}
	}

	if (recvbuflen) {
		*recvbuflen =  rbuflen;
		r = *recvbuflen;
	}

err:
	sc_unlock(card);
	LOG_FUNC_RETURN(card->ctx, r);
}
