epass2003_sm_unwrap_apdu(struct sc_card *card, struct sc_apdu *sm, struct sc_apdu *plain)
{
	int r;
	size_t len = 0;
	epass2003_exdata *exdata = NULL;

	if (!card->drv_data) 
		return SC_ERROR_INVALID_ARGUMENTS;

	exdata = (epass2003_exdata *)card->drv_data;

	LOG_FUNC_CALLED(card->ctx);

 	r = sc_check_sw(card, sm->sw1, sm->sw2);
 	if (r == SC_SUCCESS) {
 		if (exdata->sm) {
			len = plain->resplen;
 			if (0 != decrypt_response(card, sm->resp, sm->resplen, plain->resp, &len))
 				return SC_ERROR_CARD_CMD_FAILED;
 		}
		else {
			memcpy(plain->resp, sm->resp, sm->resplen);
			len = sm->resplen;
		}
	}

	plain->resplen = len;
	plain->sw1 = sm->sw1;
	plain->sw2 = sm->sw2;

	sc_log(card->ctx,
	       "unwrapped APDU: resplen %"SC_FORMAT_LEN_SIZE_T"u, SW %02X%02X",
	       plain->resplen, plain->sw1, plain->sw2);
	LOG_FUNC_RETURN(card->ctx, SC_SUCCESS);
}
