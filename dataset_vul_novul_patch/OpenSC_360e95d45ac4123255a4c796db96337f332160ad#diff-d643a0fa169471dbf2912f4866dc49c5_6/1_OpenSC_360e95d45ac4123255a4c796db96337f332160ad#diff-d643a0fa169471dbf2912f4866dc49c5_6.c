static int sc_pkcs15emu_sc_hsm_init (sc_pkcs15_card_t * p15card)
{
	sc_card_t *card = p15card->card;
	sc_hsm_private_data_t *priv = (sc_hsm_private_data_t *) card->drv_data;
	sc_file_t *file = NULL;
	sc_path_t path;
	u8 filelist[MAX_EXT_APDU_LENGTH];
	int filelistlength;
	int r, i;
	sc_cvc_t devcert;
	struct sc_app_info *appinfo;
	struct sc_pkcs15_auth_info pin_info;
	struct sc_pkcs15_object pin_obj;
	struct sc_pin_cmd_data pindata;
	u8 efbin[1024];
	u8 *ptr;
	size_t len;

	LOG_FUNC_CALLED(card->ctx);

	appinfo = calloc(1, sizeof(struct sc_app_info));

	if (appinfo == NULL) {
		LOG_FUNC_RETURN(card->ctx, SC_ERROR_OUT_OF_MEMORY);
	}

	appinfo->aid = sc_hsm_aid;

	appinfo->ddo.aid = sc_hsm_aid;
	p15card->app = appinfo;

	sc_path_set(&path, SC_PATH_TYPE_DF_NAME, sc_hsm_aid.value, sc_hsm_aid.len, 0, 0);
	r = sc_select_file(card, &path, &file);
	LOG_TEST_RET(card->ctx, r, "Could not select SmartCard-HSM application");

	p15card->card->version.hw_major = 24;	/* JCOP 2.4.1r3 */
	p15card->card->version.hw_minor = 13;
	if (file && file->prop_attr && file->prop_attr_len >= 2) {
		p15card->card->version.fw_major = file->prop_attr[file->prop_attr_len - 2];
		p15card->card->version.fw_minor = file->prop_attr[file->prop_attr_len - 1];
	}

	sc_file_free(file);

	/* Read device certificate to determine serial number */
	if (priv->EF_C_DevAut && priv->EF_C_DevAut_len) {
		ptr = priv->EF_C_DevAut;
		len = priv->EF_C_DevAut_len;
	} else {
		len = sizeof efbin;
 		r = read_file(p15card, (u8 *) "\x2F\x02", efbin, &len, 1);
 		LOG_TEST_RET(card->ctx, r, "Skipping optional EF.C_DevAut");
 
		/* save EF_C_DevAut for further use */
		ptr = realloc(priv->EF_C_DevAut, len);
		if (ptr) {
			memcpy(ptr, efbin, len);
			priv->EF_C_DevAut = ptr;
			priv->EF_C_DevAut_len = len;
 		}
 
 		ptr = efbin;
	}

	memset(&devcert, 0 ,sizeof(devcert));
	r = sc_pkcs15emu_sc_hsm_decode_cvc(p15card, (const u8 **)&ptr, &len, &devcert);
	LOG_TEST_RET(card->ctx, r, "Could not decode EF.C_DevAut");

	sc_pkcs15emu_sc_hsm_read_tokeninfo(p15card);

	if (p15card->tokeninfo->label == NULL) {
		if (p15card->card->type == SC_CARD_TYPE_SC_HSM_GOID
				|| p15card->card->type == SC_CARD_TYPE_SC_HSM_SOC) {
			p15card->tokeninfo->label = strdup("GoID");
		} else {
			p15card->tokeninfo->label = strdup("SmartCard-HSM");
		}
		if (p15card->tokeninfo->label == NULL)
			LOG_FUNC_RETURN(card->ctx, SC_ERROR_OUT_OF_MEMORY);
	}

	if ((p15card->tokeninfo->manufacturer_id != NULL) && !strcmp("(unknown)", p15card->tokeninfo->manufacturer_id)) {
		free(p15card->tokeninfo->manufacturer_id);
		p15card->tokeninfo->manufacturer_id = NULL;
	}

	if (p15card->tokeninfo->manufacturer_id == NULL) {
		if (p15card->card->type == SC_CARD_TYPE_SC_HSM_GOID
				|| p15card->card->type == SC_CARD_TYPE_SC_HSM_SOC) {
			p15card->tokeninfo->manufacturer_id = strdup("Bundesdruckerei GmbH");
		} else {
			p15card->tokeninfo->manufacturer_id = strdup("www.CardContact.de");
		}
		if (p15card->tokeninfo->manufacturer_id == NULL)
			LOG_FUNC_RETURN(card->ctx, SC_ERROR_OUT_OF_MEMORY);
	}

	appinfo->label = strdup(p15card->tokeninfo->label);
	if (appinfo->label == NULL)
		LOG_FUNC_RETURN(card->ctx, SC_ERROR_OUT_OF_MEMORY);

	len = strnlen(devcert.chr, sizeof devcert.chr);		/* Strip last 5 digit sequence number from CHR */
	assert(len >= 8);
	len -= 5;

	p15card->tokeninfo->serial_number = calloc(len + 1, 1);
	if (p15card->tokeninfo->serial_number == NULL)
		LOG_FUNC_RETURN(card->ctx, SC_ERROR_OUT_OF_MEMORY);

	memcpy(p15card->tokeninfo->serial_number, devcert.chr, len);
	*(p15card->tokeninfo->serial_number + len) = 0;

	sc_hsm_set_serialnr(card, p15card->tokeninfo->serial_number);

	sc_pkcs15emu_sc_hsm_free_cvc(&devcert);

	memset(&pin_info, 0, sizeof(pin_info));
	memset(&pin_obj, 0, sizeof(pin_obj));

	pin_info.auth_id.len = 1;
	pin_info.auth_id.value[0] = 1;
	pin_info.path.aid = sc_hsm_aid;
	pin_info.auth_type = SC_PKCS15_PIN_AUTH_TYPE_PIN;
	pin_info.attrs.pin.reference = 0x81;
	pin_info.attrs.pin.flags = SC_PKCS15_PIN_FLAG_LOCAL|SC_PKCS15_PIN_FLAG_INITIALIZED|SC_PKCS15_PIN_FLAG_EXCHANGE_REF_DATA;
	pin_info.attrs.pin.type = SC_PKCS15_PIN_TYPE_ASCII_NUMERIC;
	pin_info.attrs.pin.min_length = 6;
	pin_info.attrs.pin.stored_length = 0;
	pin_info.attrs.pin.max_length = 15;
	pin_info.attrs.pin.pad_char = '\0';
	pin_info.tries_left = 3;
	pin_info.max_tries = 3;

	pin_obj.auth_id.len = 1;
	pin_obj.auth_id.value[0] = 2;
	strlcpy(pin_obj.label, "UserPIN", sizeof(pin_obj.label));
	pin_obj.flags = SC_PKCS15_CO_FLAG_PRIVATE|SC_PKCS15_CO_FLAG_MODIFIABLE;

	r = sc_pkcs15emu_add_pin_obj(p15card, &pin_obj, &pin_info);
	if (r < 0)
		LOG_FUNC_RETURN(card->ctx, r);

	memset(&pin_info, 0, sizeof(pin_info));
	memset(&pin_obj, 0, sizeof(pin_obj));

	pin_info.auth_id.len = 1;
	pin_info.auth_id.value[0] = 2;
	pin_info.path.aid = sc_hsm_aid;
	pin_info.auth_type = SC_PKCS15_PIN_AUTH_TYPE_PIN;
	pin_info.attrs.pin.reference = 0x88;
	pin_info.attrs.pin.flags = SC_PKCS15_PIN_FLAG_LOCAL|SC_PKCS15_PIN_FLAG_INITIALIZED|SC_PKCS15_PIN_FLAG_UNBLOCK_DISABLED|SC_PKCS15_PIN_FLAG_SO_PIN;
	pin_info.attrs.pin.type = SC_PKCS15_PIN_TYPE_BCD;
	pin_info.attrs.pin.min_length = 16;
	pin_info.attrs.pin.stored_length = 0;
	pin_info.attrs.pin.max_length = 16;
	pin_info.attrs.pin.pad_char = '\0';
	pin_info.tries_left = 15;
	pin_info.max_tries = 15;

	strlcpy(pin_obj.label, "SOPIN", sizeof(pin_obj.label));
	pin_obj.flags = SC_PKCS15_CO_FLAG_PRIVATE;

	r = sc_pkcs15emu_add_pin_obj(p15card, &pin_obj, &pin_info);
	if (r < 0)
		LOG_FUNC_RETURN(card->ctx, r);


	if (card->type == SC_CARD_TYPE_SC_HSM_SOC
			|| card->type == SC_CARD_TYPE_SC_HSM_GOID) {
		/* SC-HSM of this type always has a PIN-Pad */
		r = SC_SUCCESS;
	} else {
		memset(&pindata, 0, sizeof(pindata));
		pindata.cmd = SC_PIN_CMD_GET_INFO;
		pindata.pin_type = SC_AC_CHV;
		pindata.pin_reference = 0x85;

		r = sc_pin_cmd(card, &pindata, NULL);
	}
	if (r == SC_ERROR_DATA_OBJECT_NOT_FOUND) {
		memset(&pindata, 0, sizeof(pindata));
		pindata.cmd = SC_PIN_CMD_GET_INFO;
		pindata.pin_type = SC_AC_CHV;
		pindata.pin_reference = 0x86;

		r = sc_pin_cmd(card, &pindata, NULL);
	}

	if ((r != SC_ERROR_DATA_OBJECT_NOT_FOUND) && (r != SC_ERROR_INCORRECT_PARAMETERS))
		card->caps |= SC_CARD_CAP_PROTECTED_AUTHENTICATION_PATH;


	filelistlength = sc_list_files(card, filelist, sizeof(filelist));
	LOG_TEST_RET(card->ctx, filelistlength, "Could not enumerate file and key identifier");

	for (i = 0; i < filelistlength; i += 2) {
		switch(filelist[i]) {
		case KEY_PREFIX:
			r = sc_pkcs15emu_sc_hsm_add_prkd(p15card, filelist[i + 1]);
			break;
		case DCOD_PREFIX:
			r = sc_pkcs15emu_sc_hsm_add_dcod(p15card, filelist[i + 1]);
			break;
		case CD_PREFIX:
			r = sc_pkcs15emu_sc_hsm_add_cd(p15card, filelist[i + 1]);
			break;
		}
		if (r != SC_SUCCESS) {
			sc_log(card->ctx, "Error %d adding elements to framework", r);
		}
	}

	LOG_FUNC_RETURN(card->ctx, SC_SUCCESS);
}
