static int cac_read_binary(sc_card_t *card, unsigned int idx,
		unsigned char *buf, size_t count, unsigned long flags)
{
	cac_private_data_t * priv = CAC_DATA(card);
	int r = 0;
	u8 *tl = NULL, *val = NULL;
	u8 *tl_ptr, *val_ptr, *tlv_ptr, *tl_start;
	u8 *cert_ptr;
	size_t tl_len, val_len, tlv_len;
	size_t len, tl_head_len, cert_len;
	u8 cert_type, tag;

	SC_FUNC_CALLED(card->ctx, SC_LOG_DEBUG_VERBOSE);

	/* if we didn't return it all last time, return the remainder */
	if (priv->cached) {
		sc_debug(card->ctx, SC_LOG_DEBUG_NORMAL,
			 "returning cached value idx=%d count=%"SC_FORMAT_LEN_SIZE_T"u",
			 idx, count);
		if (idx > priv->cache_buf_len) {
			SC_FUNC_RETURN(card->ctx, SC_LOG_DEBUG_NORMAL, SC_ERROR_FILE_END_REACHED);
		}
		len = MIN(count, priv->cache_buf_len-idx);
		memcpy(buf, &priv->cache_buf[idx], len);
		SC_FUNC_RETURN(card->ctx, SC_LOG_DEBUG_NORMAL, len);
	}

	sc_debug(card->ctx, SC_LOG_DEBUG_NORMAL,
		 "clearing cache idx=%d count=%"SC_FORMAT_LEN_SIZE_T"u",
		 idx, count);
	if (priv->cache_buf) {
		free(priv->cache_buf);
		priv->cache_buf = NULL;
		priv->cache_buf_len = 0;
	}


	if (priv->object_type <= 0)
		 SC_FUNC_RETURN(card->ctx, SC_LOG_DEBUG_NORMAL, SC_ERROR_INTERNAL);

	r = cac_read_file(card, CAC_FILE_TAG, &tl, &tl_len);
	if (r < 0)  {
		goto done;
	}

	r = cac_read_file(card, CAC_FILE_VALUE, &val, &val_len);
	if (r < 0)
		goto done;

	switch (priv->object_type) {
	case CAC_OBJECT_TYPE_TLV_FILE:
		tlv_len = tl_len + val_len;
		priv->cache_buf = malloc(tlv_len);
		if (priv->cache_buf == NULL) {
			r = SC_ERROR_OUT_OF_MEMORY;
			goto done;
		}
		priv->cache_buf_len = tlv_len;

		for (tl_ptr = tl, val_ptr=val, tlv_ptr = priv->cache_buf;
				tl_len >= 2 && tlv_len > 0;
				val_len -= len, tlv_len -= len, val_ptr += len, tlv_ptr += len) {
			/* get the tag and the length */
			tl_start = tl_ptr;
			if (sc_simpletlv_read_tag(&tl_ptr, tl_len, &tag, &len) != SC_SUCCESS)
				break;
			tl_head_len = (tl_ptr - tl_start);
			sc_simpletlv_put_tag(tag, len, tlv_ptr, tlv_len, &tlv_ptr);
			tlv_len -= tl_head_len;
			tl_len -= tl_head_len;

			/* don't crash on bad data */
			if (val_len < len) {
				len = val_len;
			}
			/* if we run out of return space, truncate */
			if (tlv_len < len) {
				len = tlv_len;
			}
			memcpy(tlv_ptr, val_ptr, len);
		}
		break;

	case CAC_OBJECT_TYPE_CERT:
		/* read file */
		sc_debug(card->ctx, SC_LOG_DEBUG_NORMAL,
			 " obj= cert_file, val_len=%"SC_FORMAT_LEN_SIZE_T"u (0x%04"SC_FORMAT_LEN_SIZE_T"x)",
			 val_len, val_len);
 		cert_len = 0;
 		cert_ptr = NULL;
 		cert_type = 0;
		for (tl_ptr = tl, val_ptr=val; tl_len >= 2;
				val_len -= len, val_ptr += len, tl_len -= tl_head_len) {
 			tl_start = tl_ptr;
 			if (sc_simpletlv_read_tag(&tl_ptr, tl_len, &tag, &len) != SC_SUCCESS)
 				break;
 			tl_head_len = tl_ptr - tl_start;
 			if (tag == CAC_TAG_CERTIFICATE) {
 				cert_len = len;
 				cert_ptr = val_ptr;
			}
			if (tag == CAC_TAG_CERTINFO) {
				if ((len >= 1) && (val_len >=1)) {
					cert_type = *val_ptr;
				}
			}
			if (tag == CAC_TAG_MSCUID) {
				sc_log_hex(card->ctx, "MSCUID", val_ptr, len);
			}
			if ((val_len < len) || (tl_len < tl_head_len)) {
				break;
			}
		}
		/* if the info byte is 1, then the cert is compressed, decompress it */
		if ((cert_type & 0x3) == 1) {
#ifdef ENABLE_ZLIB
			r = sc_decompress_alloc(&priv->cache_buf, &priv->cache_buf_len,
				cert_ptr, cert_len, COMPRESSION_AUTO);
#else
			sc_log(card->ctx, "CAC compression not supported, no zlib");
			r = SC_ERROR_NOT_SUPPORTED;
#endif
			if (r)
				goto done;
		} else if (cert_len > 0) {
			priv->cache_buf = malloc(cert_len);
			if (priv->cache_buf == NULL) {
				r = SC_ERROR_OUT_OF_MEMORY;
				goto done;
			}
			priv->cache_buf_len = cert_len;
			memcpy(priv->cache_buf, cert_ptr, cert_len);
		} else {
			sc_log(card->ctx, "Can't read zero-length certificate");
			goto done;
		}
		break;
	case CAC_OBJECT_TYPE_GENERIC:
		/* TODO
		 * We have some two buffers in unknown encoding that we
		 * need to present in PKCS#15 layer.
		 */
	default:
		/* Unknown object type */
		sc_log(card->ctx, "Unknown object type: %x", priv->object_type);
		r = SC_ERROR_INTERNAL;
		goto done;
	}

	/* OK we've read the data, now copy the required portion out to the callers buffer */
	priv->cached = 1;
	len = MIN(count, priv->cache_buf_len-idx);
	memcpy(buf, &priv->cache_buf[idx], len);
	r = len;
done:
	if (tl)
		free(tl);
	if (val)
		free(val);
	SC_FUNC_RETURN(card->ctx, SC_LOG_DEBUG_NORMAL, r);
}
