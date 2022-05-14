static int piv_match_card_continued(sc_card_t *card)
{
	int i;
	int type  = -1;
	piv_private_data_t *priv = NULL;
	int saved_type = card->type;

	/* Since we send an APDU, the card's logout function may be called...
	 * however it may be in dirty memory */
	card->ops->logout = NULL;

	/* piv_match_card may be called with card->type, set by opensc.conf */
	/* user provide card type must be one we know */
	switch (card->type) {
		case -1:
		case SC_CARD_TYPE_PIV_II_GENERIC:
		case SC_CARD_TYPE_PIV_II_HIST:
		case SC_CARD_TYPE_PIV_II_NEO:
		case SC_CARD_TYPE_PIV_II_YUBIKEY4:
		case SC_CARD_TYPE_PIV_II_GI_DE:
			type = card->type;
			break;
		default:
			return 0; /* can not handle the card */
	}
	if (type == -1) {

		/*
		 *try to identify card by ATR or historical data in ATR
		 * currently all PIV card will respond to piv_find_aid
		 * the same. But in future may need to know card type first,
		 * so do it here.
		 */

		if (card->reader->atr_info.hist_bytes != NULL) {
			if (card->reader->atr_info.hist_bytes_len == 8 &&
					!(memcmp(card->reader->atr_info.hist_bytes, "Yubikey4", 8))) {
				type = SC_CARD_TYPE_PIV_II_YUBIKEY4;
			}
			else if (card->reader->atr_info.hist_bytes_len >= 7 &&
					!(memcmp(card->reader->atr_info.hist_bytes, "Yubikey", 7))) {
				type = SC_CARD_TYPE_PIV_II_NEO;
			}
			/*
			 * https://csrc.nist.gov/csrc/media/projects/cryptographic-module-validation-program/documents/security-policies/140sp1239.pdf
			 * lists 2 ATRS with historical bytes:
			 *   73 66 74 65 2D 63 64 30 38 30
 			 *   73 66 74 65 20 63 64 31 34 34
 			 * will check for 73 66 74 65
 			 */
			else if (card->reader->atr_info.hist_bytes_len >= 4 &&
					!(memcmp(card->reader->atr_info.hist_bytes, "sfte", 4))) {
 				type = SC_CARD_TYPE_PIV_II_GI_DE;
 			}
 
			else if (card->reader->atr_info.hist_bytes[0] == 0x80u) { /* compact TLV */
 				size_t datalen;
 				const u8 *data = sc_compacttlv_find_tag(card->reader->atr_info.hist_bytes + 1,
 									card->reader->atr_info.hist_bytes_len - 1,
									0xF0, &datalen);

				if (data != NULL) {
					int k;

					for (k = 0; piv_aids[k].len_long != 0; k++) {
						if (datalen == piv_aids[k].len_long
							&& !memcmp(data, piv_aids[k].value, datalen)) {
							type = SC_CARD_TYPE_PIV_II_HIST;
							break;
						}
					}
				}
			}
		}
		if (type == -1)
			type = SC_CARD_TYPE_PIV_II_GENERIC;
	}

	/* allocate and init basic fields */

	priv = calloc(1, sizeof(piv_private_data_t));

	if (!priv)
		LOG_FUNC_RETURN(card->ctx, SC_ERROR_OUT_OF_MEMORY);

	if (card->type == -1)
		card->type = type;

	card->drv_data = priv; /* will free if no match, or pass on to piv_init */
	priv->aid_file = sc_file_new();
	priv->selected_obj = -1;
	priv->pin_preference = 0x80; /* 800-73-3 part 1, table 3 */
	priv->logged_in = SC_PIN_STATE_UNKNOWN;
	priv->tries_left = 10; /* will assume OK at start */
	priv->pstate = PIV_STATE_MATCH;

	/* Some objects will only be present if History object says so */
	for (i=0; i < PIV_OBJ_LAST_ENUM -1; i++)
		if(piv_objects[i].flags & PIV_OBJECT_NOT_PRESENT)
			priv->obj_cache[i].flags |= PIV_OBJ_CACHE_NOT_PRESENT;

	sc_lock(card);

	/*
	 * detect if active AID is PIV. NIST 800-73 says Only one PIV application per card
	 * and PIV must be the default application
	 * This can avoid doing doing a select_aid and losing the login state on some cards
	 * We may get interference on some cards by other drivers trying SELECT_AID before
	 * we get to see if PIV application is still active.
	 * putting PIV driver first might help. 
	 * This may fail if the wrong AID is active
	 */
	i = piv_find_discovery(card);

	if (i < 0) {
		/* Detect by selecting applet */
		sc_file_t aidfile;

		i = piv_find_aid(card, &aidfile);
	}

	if (i >= 0) {
		/*
		 * We now know PIV AID is active, test DISCOVERY object 
		 * Some CAC cards with PIV don't support DISCOVERY and return 
		 * SC_ERROR_INCORRECT_PARAMETERS. Any error other then 
		 * SC_ERROR_FILE_NOT_FOUND means we cannot use discovery 
		 * to test for active AID.
		 */
		int i7e = piv_find_discovery(card);

		if (i7e != 0 && i7e !=  SC_ERROR_FILE_NOT_FOUND) {
			priv->card_issues |= CI_DISCOVERY_USELESS;
			priv->obj_cache[PIV_OBJ_DISCOVERY].flags |= PIV_OBJ_CACHE_NOT_PRESENT;
		}
	}


	if (i < 0) {
		/* don't match. Does not have a PIV applet. */
		sc_unlock(card);
		piv_finish(card);
		card->type = saved_type;
		return 0;
	}

	/* Matched, caller will use or free priv and sc_lock as needed */
	priv->pstate=PIV_STATE_INIT;
	return 1; /* match */
}
