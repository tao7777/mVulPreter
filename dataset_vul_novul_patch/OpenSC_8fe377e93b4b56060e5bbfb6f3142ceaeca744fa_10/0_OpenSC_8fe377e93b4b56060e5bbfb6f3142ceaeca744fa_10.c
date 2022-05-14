iasecc_get_serialnr(struct sc_card *card, struct sc_serial_number *serial)
{
	struct sc_context *ctx = card->ctx;
	struct sc_iin *iin = &card->serialnr.iin;
	struct sc_apdu apdu;
	unsigned char rbuf[0xC0];
	size_t ii, offs;
	int rv;

	LOG_FUNC_CALLED(ctx);
	if (card->serialnr.len)
		goto end;

	memset(&card->serialnr, 0, sizeof(card->serialnr));

	sc_format_apdu(card, &apdu, SC_APDU_CASE_2_SHORT, 0xB0, 0x80 | IASECC_SFI_EF_SN, 0);
	apdu.le = sizeof(rbuf);
	apdu.resp = rbuf;
	apdu.resplen = sizeof(rbuf);

	rv = sc_transmit_apdu(card, &apdu);
	LOG_TEST_RET(ctx, rv, "APDU transmit failed");
	rv = sc_check_sw(card, apdu.sw1, apdu.sw2);
	LOG_TEST_RET(ctx, rv, "Get 'serial number' data failed");

	if (rbuf[0] != ISO7812_PAN_SN_TAG)
		LOG_TEST_RET(ctx, SC_ERROR_UNKNOWN_DATA_RECEIVED, "serial number parse error");

	iin->mii = (rbuf[2] >> 4) & 0x0F;

	iin->country = 0;
	for (ii=5; ii<8; ii++)   {
		iin->country *= 10;
		iin->country += (rbuf[ii/2] >> ((ii & 0x01) ? 0 : 4)) & 0x0F;
	}

	iin->issuer_id = 0;
	for (ii=8; ii<10; ii++)   {
		iin->issuer_id *= 10;
		iin->issuer_id += (rbuf[ii/2] >> (ii & 0x01 ? 0 : 4)) & 0x0F;
	}

	offs = rbuf[1] > 8 ? rbuf[1] - 8 : 0;
 	if (card->type == SC_CARD_TYPE_IASECC_SAGEM)   {
 		/* 5A 0A 92 50 00 20 10 10 25 00 01 3F */
 		/*            00 02 01 01 02 50 00 13  */
		for (ii=0; (ii < rbuf[1] - offs) && (ii + offs + 2 < sizeof(rbuf)); ii++)
 			*(card->serialnr.value + ii) = ((rbuf[ii + offs + 1] & 0x0F) << 4)
 				+ ((rbuf[ii + offs + 2] & 0xF0) >> 4) ;
 		card->serialnr.len = ii;
	}
	else   {
		for (ii=0; ii < rbuf[1] - offs; ii++)
			*(card->serialnr.value + ii) = rbuf[ii + offs + 2];
		card->serialnr.len = ii;
	}

	do  {
		char txt[0x200];

		for (ii=0;ii<card->serialnr.len;ii++)
			sprintf(txt + ii*2, "%02X", *(card->serialnr.value + ii));

		sc_log(ctx, "serial number '%s'; mii %i; country %i; issuer_id %li", txt, iin->mii, iin->country, iin->issuer_id);
	} while(0);

end:
	if (serial)
		memcpy(serial, &card->serialnr, sizeof(*serial));

	LOG_FUNC_RETURN(ctx, SC_SUCCESS);
}
