pgp_enumerate_blob(sc_card_t *card, pgp_blob_t *blob)
{
	const u8	*in;
	int		r;

	if (blob->files != NULL)
		return SC_SUCCESS;

	if ((r = pgp_read_blob(card, blob)) < 0)
		return r;

	in = blob->data;

	while ((int) blob->len > (in - blob->data)) {
		unsigned int	cla, tag, tmptag;
		size_t		len;
 		const u8	*data = in;
 		pgp_blob_t	*new;
 
 		r = sc_asn1_read_tag(&data, blob->len - (in - blob->data),
 					&cla, &tag, &len);
 		if (r < 0 || data == NULL) {
			sc_debug(card->ctx, SC_LOG_DEBUG_NORMAL,
				 "Unexpected end of contents\n");
 			return SC_ERROR_OBJECT_NOT_VALID;
 		}
 
 		/* undo ASN1's split of tag & class */
 		for (tmptag = tag; tmptag > 0x0FF; tmptag >>= 8) {
 			cla <<= 8;
		}
		tag |= cla;

		/* Awful hack for composite DOs that have
		 * a TLV with the DO's id encompassing the
		 * entire blob. Example: Yubikey Neo */
		if (tag == blob->id) {
			in = data;
			continue;
		}

		/* create fake file system hierarchy by
		 * using constructed DOs as DF */
		if ((new = pgp_new_blob(card, blob, tag, sc_file_new())) == NULL)
			return SC_ERROR_OUT_OF_MEMORY;
		pgp_set_blob(new, data, len);
		in = data + len;
	}

	return SC_SUCCESS;
}
