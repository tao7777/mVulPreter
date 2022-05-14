static void traverse_for_entities(
	const char *old,
	size_t oldlen,
	char *ret, /* should have allocated TRAVERSE_FOR_ENTITIES_EXPAND_SIZE(olden) */
	size_t *retlen,
	int all,
	int flags,
	const entity_ht *inv_map,
	enum entity_charset charset)
{
	const char *p,
			   *lim;
	char	   *q;
	int doctype = flags & ENT_HTML_DOC_TYPE_MASK;

	lim = old + oldlen; /* terminator address */
	assert(*lim == '\0');

	for (p = old, q = ret; p < lim;) {
		unsigned code, code2 = 0;
		const char *next = NULL; /* when set, next > p, otherwise possible inf loop */

		/* Shift JIS, Big5 and HKSCS use multi-byte encodings where an
		 * ASCII range byte can be part of a multi-byte sequence.
		 * However, they start at 0x40, therefore if we find a 0x26 byte,
		 * we're sure it represents the '&' character. */

		/* assumes there are no single-char entities */
		if (p[0] != '&' || (p + 3 >= lim)) {
			*(q++) = *(p++);
			continue;
		}

		/* now p[3] is surely valid and is no terminator */

		/* numerical entity */
		if (p[1] == '#') {
			next = &p[2];
			if (process_numeric_entity(&next, &code) == FAILURE)
				goto invalid_code;

			/* If we're in htmlspecialchars_decode, we're only decoding entities
			 * that represent &, <, >, " and '. Is this one of them? */
			if (!all && (code > 63U ||
					stage3_table_be_apos_00000[code].data.ent.entity == NULL))
 				goto invalid_code;
 
 			/* are we allowed to decode this entity in this document type?
			 * HTML 5 is the only that has a character that cannot be used in
 			 * a numeric entity but is allowed literally (U+000D). The
 			 * unoptimized version would be ... || !numeric_entity_is_allowed(code) */
 			if (!unicode_cp_is_allowed(code, doctype) ||
					(doctype == ENT_HTML_DOC_HTML5 && code == 0x0D))
				goto invalid_code;
		} else {
			const char *start;
			size_t ent_len;

			next = &p[1];
			start = next;

			if (process_named_entity_html(&next, &start, &ent_len) == FAILURE)
				goto invalid_code;

			if (resolve_named_entity_html(start, ent_len, inv_map, &code, &code2) == FAILURE) {
				if (doctype == ENT_HTML_DOC_XHTML && ent_len == 4 && start[0] == 'a'
							&& start[1] == 'p' && start[2] == 'o' && start[3] == 's') {
					/* uses html4 inv_map, which doesn't include apos;. This is a
					 * hack to support it */
					code = (unsigned) '\'';
				} else {
					goto invalid_code;
 				}
 			}
 		}

 		assert(*next == ';');

 		if (((code == '\'' && !(flags & ENT_HTML_QUOTE_SINGLE)) ||
 				(code == '"' && !(flags & ENT_HTML_QUOTE_DOUBLE)))
 				/* && code2 == '\0' always true for current maps */)
			goto invalid_code;

		/* UTF-8 doesn't need mapping (ISO-8859-1 doesn't either, but
		 * the call is needed to ensure the codepoint <= U+00FF)  */
		if (charset != cs_utf_8) {
			/* replace unicode code point */
			if (map_from_unicode(code, charset, &code) == FAILURE || code2 != 0)
				goto invalid_code; /* not representable in target charset */
		}

		q += write_octet_sequence(q, charset, code);
		if (code2) {
			q += write_octet_sequence(q, charset, code2);
		}

		/* jump over the valid entity; may go beyond size of buffer; np */
		p = next + 1;
		continue;

invalid_code:
		for (; p < next; p++) {
 			*(q++) = *p;
 		}
 	}

 	*q = '\0';
 	*retlen = (size_t)(q - ret);
 }
