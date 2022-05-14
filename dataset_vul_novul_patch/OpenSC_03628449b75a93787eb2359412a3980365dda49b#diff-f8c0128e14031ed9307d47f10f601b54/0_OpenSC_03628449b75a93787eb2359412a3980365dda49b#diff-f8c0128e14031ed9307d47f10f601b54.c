iasecc_select_file(struct sc_card *card, const struct sc_path *path,
		 struct sc_file **file_out)
{
	struct sc_context *ctx = card->ctx;
	struct sc_path lpath;
	int cache_valid = card->cache.valid, df_from_cache = 0;
	int rv, ii;

	LOG_FUNC_CALLED(ctx);
	memcpy(&lpath, path, sizeof(struct sc_path));
	if (file_out)
		*file_out = NULL;

	sc_log(ctx,
	       "iasecc_select_file(card:%p) path.len %"SC_FORMAT_LEN_SIZE_T"u; path.type %i; aid_len %"SC_FORMAT_LEN_SIZE_T"u",
	       card, path->len, path->type, path->aid.len);
 	sc_log(ctx, "iasecc_select_file() path:%s", sc_print_path(path));
 
 	sc_print_cache(card);
	if (path->type != SC_PATH_TYPE_DF_NAME
			&& lpath.len >= 2
			&& lpath.value[0] == 0x3F && lpath.value[1] == 0x00)   {
 		sc_log(ctx, "EF.ATR(aid:'%s')", card->ef_atr ? sc_dump_hex(card->ef_atr->aid.value, card->ef_atr->aid.len) : "");
 
 		rv = iasecc_select_mf(card, file_out);
 		LOG_TEST_RET(ctx, rv, "MF selection error");
 
		memmove(&lpath.value[0], &lpath.value[2], lpath.len - 2);
		lpath.len -=  2;
 	}
 
 	if (lpath.aid.len)	{
		struct sc_file *file = NULL;
		struct sc_path ppath;

		sc_log(ctx,
		       "iasecc_select_file() select parent AID:%p/%"SC_FORMAT_LEN_SIZE_T"u",
		       lpath.aid.value, lpath.aid.len);
		sc_log(ctx, "iasecc_select_file() select parent AID:%s", sc_dump_hex(lpath.aid.value, lpath.aid.len));
		memset(&ppath, 0, sizeof(ppath));
		memcpy(ppath.value, lpath.aid.value, lpath.aid.len);
		ppath.len = lpath.aid.len;
		ppath.type = SC_PATH_TYPE_DF_NAME;

		if (card->cache.valid && card->cache.current_df
				&& card->cache.current_df->path.len == lpath.aid.len
				&& !memcmp(card->cache.current_df->path.value, lpath.aid.value, lpath.aid.len))
			df_from_cache = 1;

		rv = iasecc_select_file(card, &ppath, &file);
		LOG_TEST_RET(ctx, rv, "select AID path failed");

		if (file_out)
			*file_out = file;
		else
		   sc_file_free(file);

		if (lpath.type == SC_PATH_TYPE_DF_NAME)
			lpath.type = SC_PATH_TYPE_FROM_CURRENT;
	}

	if (lpath.type == SC_PATH_TYPE_PATH)
		lpath.type = SC_PATH_TYPE_FROM_CURRENT;

	if (!lpath.len)
		LOG_FUNC_RETURN(ctx, SC_SUCCESS);

	sc_print_cache(card);

	if (card->cache.valid && card->cache.current_df && lpath.type == SC_PATH_TYPE_DF_NAME
			&& card->cache.current_df->path.len == lpath.len
			&& !memcmp(card->cache.current_df->path.value, lpath.value, lpath.len))   {
		sc_log(ctx, "returns current DF path %s", sc_print_path(&card->cache.current_df->path));
		if (file_out)   {
			sc_file_free(*file_out);
			sc_file_dup(file_out, card->cache.current_df);
		}

		sc_print_cache(card);
		LOG_FUNC_RETURN(ctx, SC_SUCCESS);
	}

	do   {
		struct sc_apdu apdu;
		struct sc_file *file = NULL;
		unsigned char rbuf[SC_MAX_APDU_BUFFER_SIZE];
		int pathlen = lpath.len;

		sc_format_apdu(card, &apdu, SC_APDU_CASE_4_SHORT, 0xA4, 0x00, 0x00);

		if (card->type != SC_CARD_TYPE_IASECC_GEMALTO
				&& card->type != SC_CARD_TYPE_IASECC_OBERTHUR
				&& card->type != SC_CARD_TYPE_IASECC_SAGEM
				&& card->type != SC_CARD_TYPE_IASECC_AMOS
				&& card->type != SC_CARD_TYPE_IASECC_MI
				&& card->type != SC_CARD_TYPE_IASECC_MI2)
			LOG_TEST_RET(ctx, SC_ERROR_NOT_SUPPORTED, "Unsupported card");

		if (lpath.type == SC_PATH_TYPE_FILE_ID)   {
			apdu.p1 = 0x02;
			if (card->type == SC_CARD_TYPE_IASECC_OBERTHUR)   {
				apdu.p1 = 0x01;
				apdu.p2 = 0x04;
			}
			if (card->type == SC_CARD_TYPE_IASECC_AMOS)
				apdu.p2 = 0x04;
			if (card->type == SC_CARD_TYPE_IASECC_MI)
				apdu.p2 = 0x04;
			if (card->type == SC_CARD_TYPE_IASECC_MI2)
				apdu.p2 = 0x04;
		}
		else if (lpath.type == SC_PATH_TYPE_FROM_CURRENT)  {
			apdu.p1 = 0x09;
			if (card->type == SC_CARD_TYPE_IASECC_OBERTHUR)
				apdu.p2 = 0x04;
			if (card->type == SC_CARD_TYPE_IASECC_AMOS)
				apdu.p2 = 0x04;
			if (card->type == SC_CARD_TYPE_IASECC_MI)
				apdu.p2 = 0x04;
			if (card->type == SC_CARD_TYPE_IASECC_MI2)
				apdu.p2 = 0x04;
		}
		else if (lpath.type == SC_PATH_TYPE_PARENT)   {
			apdu.p1 = 0x03;
			pathlen = 0;
			apdu.cse = SC_APDU_CASE_2_SHORT;
		}
		else if (lpath.type == SC_PATH_TYPE_DF_NAME)   {
			apdu.p1 = 0x04;
			if (card->type == SC_CARD_TYPE_IASECC_AMOS)
				apdu.p2 = 0x04;
			if (card->type == SC_CARD_TYPE_IASECC_MI2)
				apdu.p2 = 0x04;
		}
		else   {
			sc_log(ctx, "Invalid PATH type: 0x%X", lpath.type);
			LOG_TEST_RET(ctx, SC_ERROR_NOT_SUPPORTED, "iasecc_select_file() invalid PATH type");
		}

		for (ii=0; ii<2; ii++)   {
			apdu.lc = pathlen;
			apdu.data = lpath.value;
			apdu.datalen = pathlen;

			apdu.resp = rbuf;
			apdu.resplen = sizeof(rbuf);
			apdu.le = 256;

			rv = sc_transmit_apdu(card, &apdu);
			LOG_TEST_RET(ctx, rv, "APDU transmit failed");
			rv = sc_check_sw(card, apdu.sw1, apdu.sw2);
			if (rv == SC_ERROR_INCORRECT_PARAMETERS &&
					lpath.type == SC_PATH_TYPE_DF_NAME && apdu.p2 == 0x00)   {
				apdu.p2 = 0x0C;
				continue;
			}

			if (ii)   {
				/* 'SELECT AID' do not returned FCP. Try to emulate. */
				apdu.resplen = sizeof(rbuf);
				rv = iasecc_emulate_fcp(ctx, &apdu);
				LOG_TEST_RET(ctx, rv, "Failed to emulate DF FCP");
			}

			break;
		}

		/*
		 * Using of the cached DF and EF can cause problems in the multi-thread environment.
		 * FIXME: introduce config. option that invalidates this cache outside the locked card session,
		 *        (or invent something else)
		 */
		if (rv == SC_ERROR_FILE_NOT_FOUND && cache_valid && df_from_cache)   {
			sc_invalidate_cache(card);
			sc_log(ctx, "iasecc_select_file() file not found, retry without cached DF");
			if (file_out)   {
				sc_file_free(*file_out);
				*file_out = NULL;
			}
			rv = iasecc_select_file(card, path, file_out);
			LOG_FUNC_RETURN(ctx, rv);
		}

		LOG_TEST_RET(ctx, rv, "iasecc_select_file() check SW failed");

		sc_log(ctx,
		       "iasecc_select_file() apdu.resp %"SC_FORMAT_LEN_SIZE_T"u",
		       apdu.resplen);
		if (apdu.resplen)   {
			sc_log(ctx, "apdu.resp %02X:%02X:%02X...", apdu.resp[0], apdu.resp[1], apdu.resp[2]);

			switch (apdu.resp[0]) {
			case 0x62:
			case 0x6F:
				file = sc_file_new();
				if (file == NULL)
					LOG_FUNC_RETURN(ctx, SC_ERROR_OUT_OF_MEMORY);
				file->path = lpath;

				rv = iasecc_process_fci(card, file, apdu.resp, apdu.resplen);
				if (rv)
					LOG_FUNC_RETURN(ctx, rv);
				break;
			default:
				LOG_FUNC_RETURN(ctx, SC_ERROR_UNKNOWN_DATA_RECEIVED);
			}

			sc_log(ctx, "FileType %i", file->type);
			if (file->type == SC_FILE_TYPE_DF)   {
				if (card->cache.valid)
					sc_file_free(card->cache.current_df);
				card->cache.current_df = NULL;


				if (card->cache.valid)
					sc_file_free(card->cache.current_ef);
				card->cache.current_ef = NULL;

				sc_file_dup(&card->cache.current_df, file);
				card->cache.valid = 1;
			}
			else   {
				if (card->cache.valid)
					sc_file_free(card->cache.current_ef);

				card->cache.current_ef = NULL;

				sc_file_dup(&card->cache.current_ef, file);
			}

			if (file_out)   {
				sc_file_free(*file_out);
				*file_out = file;
			}
			else   {
				sc_file_free(file);
			}
		}
		else if (lpath.type == SC_PATH_TYPE_DF_NAME)   {
			sc_file_free(card->cache.current_df);
			card->cache.current_df = NULL;

			sc_file_free(card->cache.current_ef);
			card->cache.current_ef = NULL;

			card->cache.valid = 1;
		}
	} while(0);

	sc_print_cache(card);
	LOG_FUNC_RETURN(ctx, SC_SUCCESS);
}
