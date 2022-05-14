static int gemsafe_get_cert_len(sc_card_t *card)
{
	int r;
	u8  ibuf[GEMSAFE_MAX_OBJLEN];
	u8 *iptr;
	struct sc_path path;
	struct sc_file *file;
	size_t objlen, certlen;
	unsigned int ind, i=0;

	sc_format_path(GEMSAFE_PATH, &path);
	r = sc_select_file(card, &path, &file);
	if (r != SC_SUCCESS || !file)
		return SC_ERROR_INTERNAL;

	/* Initial read */
	r = sc_read_binary(card, 0, ibuf, GEMSAFE_READ_QUANTUM, 0);
	if (r < 0)
		return SC_ERROR_INTERNAL;

	/* Actual stored object size is encoded in first 2 bytes
	 * (allocated EF space is much greater!)
	 */
	objlen = (((size_t) ibuf[0]) << 8) | ibuf[1];
	sc_log(card->ctx, "Stored object is of size: %"SC_FORMAT_LEN_SIZE_T"u",
	       objlen);
	if (objlen < 1 || objlen > GEMSAFE_MAX_OBJLEN) {
	    sc_log(card->ctx, "Invalid object size: %"SC_FORMAT_LEN_SIZE_T"u",
		   objlen);
	    return SC_ERROR_INTERNAL;
	}

	/* It looks like the first thing in the block is a table of
	 * which keys are allocated. The table is small and is in the
	 * first 248 bytes. Example for a card with 10 key containers:
	 * 01 f0 00 03 03 b0 00 03     <=  1st key unallocated
	 * 01 f0 00 04 03 b0 00 04     <=  2nd key unallocated
	 * 01 fe 14 00 05 03 b0 00 05  <=  3rd key allocated
	 * 01 fe 14 01 06 03 b0 00 06  <=  4th key allocated
	 * 01 f0 00 07 03 b0 00 07     <=  5th key unallocated
	 * ...
	 * 01 f0 00 0c 03 b0 00 0c     <= 10th key unallocated
	 * For allocated keys, the fourth byte seems to indicate the
	 * default key and the fifth byte indicates the key_ref of
 	 * the private key.
 	 */
 	ind = 2; /* skip length */
	while (ibuf[ind] == 0x01) {
 		if (ibuf[ind+1] == 0xFE) {
 			gemsafe_prkeys[i].ref = ibuf[ind+4];
 			sc_log(card->ctx, "Key container %d is allocated and uses key_ref %d",
					i+1, gemsafe_prkeys[i].ref);
			ind += 9;
		}
		else {
			gemsafe_prkeys[i].label = NULL;
			gemsafe_cert[i].label = NULL;
			sc_log(card->ctx, "Key container %d is unallocated", i+1);
			ind += 8;
		}
		i++;
	}

	/* Delete additional key containers from the data structures if
	 * this card can't accommodate them.
	 */
	for (; i < gemsafe_cert_max; i++) {
		gemsafe_prkeys[i].label = NULL;
		gemsafe_cert[i].label = NULL;
	}

	/* Read entire file, then dissect in memory.
	 * Gemalto ClassicClient seems to do it the same way.
	 */
	iptr = ibuf + GEMSAFE_READ_QUANTUM;
	while ((size_t)(iptr - ibuf) < objlen) {
		r = sc_read_binary(card, iptr - ibuf, iptr,
				   MIN(GEMSAFE_READ_QUANTUM, objlen - (iptr - ibuf)), 0);
		if (r < 0) {
			sc_log(card->ctx, "Could not read cert object");
			return SC_ERROR_INTERNAL;
		}
		iptr += GEMSAFE_READ_QUANTUM;
	}

	/* Search buffer for certificates, they start with 0x3082. */
	i = 0;
	while (ind < objlen - 1) {
		if (ibuf[ind] == 0x30 && ibuf[ind+1] == 0x82) {
			/* Find next allocated key container */
			while (i < gemsafe_cert_max && gemsafe_cert[i].label == NULL)
				i++;
			if (i == gemsafe_cert_max) {
				sc_log(card->ctx, "Warning: Found orphaned certificate at offset %d", ind);
				return SC_SUCCESS;
			}
			/* DER cert len is encoded this way */
			if (ind+3 >= sizeof ibuf)
				return SC_ERROR_INVALID_DATA;
			certlen = ((((size_t) ibuf[ind+2]) << 8) | ibuf[ind+3]) + 4;
			sc_log(card->ctx,
			       "Found certificate of key container %d at offset %d, len %"SC_FORMAT_LEN_SIZE_T"u",
			       i+1, ind, certlen);
			gemsafe_cert[i].index = ind;
			gemsafe_cert[i].count = certlen;
			ind += certlen;
			i++;
		} else
			ind++;
	}

	/* Delete additional key containers from the data structures if
	 * they're missing on the card.
	 */
	for (; i < gemsafe_cert_max; i++) {
		if (gemsafe_cert[i].label) {
			sc_log(card->ctx, "Warning: Certificate of key container %d is missing", i+1);
			gemsafe_prkeys[i].label = NULL;
			gemsafe_cert[i].label = NULL;
		}
	}

	return SC_SUCCESS;
}
