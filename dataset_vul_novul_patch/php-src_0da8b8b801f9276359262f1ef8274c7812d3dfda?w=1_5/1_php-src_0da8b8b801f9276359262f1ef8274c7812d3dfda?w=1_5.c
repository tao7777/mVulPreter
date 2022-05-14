PHPAPI char *php_escape_html_entities_ex(unsigned char *old, size_t oldlen, size_t *newlen, int all, int flags, char *hint_charset, zend_bool double_encode TSRMLS_DC)
{
	size_t cursor, maxlen, len;
	char *replaced;
	enum entity_charset charset = determine_charset(hint_charset TSRMLS_CC);
	int doctype = flags & ENT_HTML_DOC_TYPE_MASK;
	entity_table_opt entity_table;
	const enc_to_uni *to_uni_table = NULL;
	const entity_ht *inv_map = NULL; /* used for !double_encode */
	/* only used if flags includes ENT_HTML_IGNORE_ERRORS or ENT_HTML_SUBSTITUTE_DISALLOWED_CHARS */
	const unsigned char *replacement = NULL;
	size_t replacement_len = 0;

	if (all) { /* replace with all named entities */
		if (CHARSET_PARTIAL_SUPPORT(charset)) {
			php_error_docref0(NULL TSRMLS_CC, E_STRICT, "Only basic entities "
				"substitution is supported for multi-byte encodings other than UTF-8; "
				"functionality is equivalent to htmlspecialchars");
		}
		LIMIT_ALL(all, doctype, charset);
	}
	entity_table = determine_entity_table(all, doctype);
	if (all && !CHARSET_UNICODE_COMPAT(charset)) {
		to_uni_table = enc_to_uni_index[charset];
	}

	if (!double_encode) {
		/* first arg is 1 because we want to identify valid named entities
		 * even if we are only encoding the basic ones */
		inv_map = unescape_inverse_map(1, flags);
	}

	if (flags & (ENT_HTML_SUBSTITUTE_ERRORS | ENT_HTML_SUBSTITUTE_DISALLOWED_CHARS)) {
		if (charset == cs_utf_8) {
			replacement = (const unsigned char*)"\xEF\xBF\xBD";
			replacement_len = sizeof("\xEF\xBF\xBD") - 1;
		} else {
			replacement = (const unsigned char*)"&#xFFFD;";
			replacement_len = sizeof("&#xFFFD;") - 1;
		}
	}
 
 	/* initial estimate */
 	if (oldlen < 64) {
		maxlen = 128;	
 	} else {
 		maxlen = 2 * oldlen;
 		if (maxlen < oldlen) {
			zend_error_noreturn(E_ERROR, "Input string is too long");
			return NULL;
		}
	}

	replaced = emalloc(maxlen + 1); /* adding 1 is safe: maxlen is even */
	len = 0;
	cursor = 0;
	while (cursor < oldlen) {
		const unsigned char *mbsequence = NULL;
		size_t mbseqlen					= 0,
		       cursor_before			= cursor;
		int status						= SUCCESS;
		unsigned int this_char			= get_next_char(charset, old, oldlen, &cursor, &status);

		/* guarantee we have at least 40 bytes to write.
		 * In HTML5, entities may take up to 33 bytes */
		if (len > maxlen - 40) { /* maxlen can never be smaller than 128 */
			replaced = safe_erealloc(replaced, maxlen , 1, 128 + 1);
			maxlen += 128;
		}

		if (status == FAILURE) {
			/* invalid MB sequence */
			if (flags & ENT_HTML_IGNORE_ERRORS) {
				continue;
			} else if (flags & ENT_HTML_SUBSTITUTE_ERRORS) {
				memcpy(&replaced[len], replacement, replacement_len);
				len += replacement_len;
				continue;
			} else {
				efree(replaced);
				*newlen = 0;
				return STR_EMPTY_ALLOC();
			}
		} else { /* SUCCESS */
			mbsequence = &old[cursor_before];
			mbseqlen = cursor - cursor_before;
		}

		if (this_char != '&') { /* no entity on this position */
			const unsigned char *rep	= NULL;
			size_t				rep_len	= 0;

			if (((this_char == '\'' && !(flags & ENT_HTML_QUOTE_SINGLE)) ||
					(this_char == '"' && !(flags & ENT_HTML_QUOTE_DOUBLE))))
				goto pass_char_through;

			if (all) { /* false that CHARSET_PARTIAL_SUPPORT(charset) */
				if (to_uni_table != NULL) {
					/* !CHARSET_UNICODE_COMPAT therefore not UTF-8; since UTF-8
					 * is the only multibyte encoding with !CHARSET_PARTIAL_SUPPORT,
					 * we're using a single byte encoding */
					map_to_unicode(this_char, to_uni_table, &this_char);
					if (this_char == 0xFFFF) /* no mapping; pass through */
						goto pass_char_through;
				}
				/* the cursor may advance */
				find_entity_for_char(this_char, charset, entity_table.ms_table, &rep,
					&rep_len, old, oldlen, &cursor);
			} else {
				find_entity_for_char_basic(this_char, entity_table.table, &rep, &rep_len);
			}

			if (rep != NULL) {
				replaced[len++] = '&';
				memcpy(&replaced[len], rep, rep_len);
				len += rep_len;
				replaced[len++] = ';';
			} else {
				/* we did not find an entity for this char.
				 * check for its validity, if its valid pass it unchanged */
				if (flags & ENT_HTML_SUBSTITUTE_DISALLOWED_CHARS) {
					if (CHARSET_UNICODE_COMPAT(charset)) {
						if (!unicode_cp_is_allowed(this_char, doctype)) {
							mbsequence = replacement;
							mbseqlen = replacement_len;
						}
					} else if (to_uni_table) {
						if (!all) /* otherwise we already did this */
							map_to_unicode(this_char, to_uni_table, &this_char);
						if (!unicode_cp_is_allowed(this_char, doctype)) {
							mbsequence = replacement;
							mbseqlen = replacement_len;
						}
					} else {
						/* not a unicode code point, unless, coincidentally, it's in
						 * the 0x20..0x7D range (except 0x5C in sjis). We know nothing
						 * about other code points, because we have no tables. Since
						 * Unicode code points in that range are not disallowed in any
						 * document type, we could do nothing. However, conversion
						 * tables frequently map 0x00-0x1F to the respective C0 code
						 * points. Let's play it safe and admit that's the case */
						if (this_char <= 0x7D &&
								!unicode_cp_is_allowed(this_char, doctype)) {
							mbsequence = replacement;
							mbseqlen = replacement_len;
						}
					}
				}
pass_char_through:
				if (mbseqlen > 1) {
					memcpy(replaced + len, mbsequence, mbseqlen);
					len += mbseqlen;
				} else {
					replaced[len++] = mbsequence[0];
				}
			}
		} else { /* this_char == '&' */
			if (double_encode) {
encode_amp:
				memcpy(&replaced[len], "&amp;", sizeof("&amp;") - 1);
				len += sizeof("&amp;") - 1;
			} else { /* no double encode */
				/* check if entity is valid */
				size_t ent_len; /* not counting & or ; */
				/* peek at next char */
				if (old[cursor] == '#') { /* numeric entity */
					unsigned code_point;
					int valid;
					char *pos = (char*)&old[cursor+1];
					valid = process_numeric_entity((const char **)&pos, &code_point);
					if (valid == FAILURE)
						goto encode_amp;
					if (flags & ENT_HTML_SUBSTITUTE_DISALLOWED_CHARS) {
						if (!numeric_entity_is_allowed(code_point, doctype))
							goto encode_amp;
					}
					ent_len = pos - (char*)&old[cursor];
				} else { /* named entity */
					/* check for vality of named entity */
					const char *start = &old[cursor],
							   *next = start;
					unsigned   dummy1, dummy2;

					if (process_named_entity_html(&next, &start, &ent_len) == FAILURE)
						goto encode_amp;
					if (resolve_named_entity_html(start, ent_len, inv_map, &dummy1, &dummy2) == FAILURE) {
						if (!(doctype == ENT_HTML_DOC_XHTML && ent_len == 4 && start[0] == 'a'
									&& start[1] == 'p' && start[2] == 'o' && start[3] == 's')) {
							/* uses html4 inv_map, which doesn't include apos;. This is a
							 * hack to support it */
							goto encode_amp;
						}
					}
				}
				/* checks passed; copy entity to result */
				/* entity size is unbounded, we may need more memory */
				/* at this point maxlen - len >= 40 */
				if (maxlen - len < ent_len + 2 /* & and ; */) {
					/* ent_len < oldlen, which is certainly <= SIZE_MAX/2 */
					replaced = safe_erealloc(replaced, maxlen, 1, ent_len + 128 + 1);
					maxlen += ent_len + 128;
				}
				replaced[len++] = '&';
				memcpy(&replaced[len], &old[cursor], ent_len);
				len += ent_len;
				replaced[len++] = ';';
				cursor += ent_len + 1;
			}
		}
	}
	replaced[len] = '\0';
	*newlen = len;

	return replaced;
}
