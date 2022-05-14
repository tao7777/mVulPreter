PHPAPI char *php_unescape_html_entities(unsigned char *old, size_t oldlen, size_t *newlen, int all, int flags, char *hint_charset TSRMLS_DC)
{
	size_t retlen;
	char *ret;
	enum entity_charset charset;
	const entity_ht *inverse_map = NULL;
	size_t new_size = TRAVERSE_FOR_ENTITIES_EXPAND_SIZE(oldlen);

	if (all) {
		charset = determine_charset(hint_charset TSRMLS_CC);
	} else {
		charset = cs_8859_1; /* charset shouldn't matter, use ISO-8859-1 for performance */
	}

	/* don't use LIMIT_ALL! */

	if (oldlen > new_size) {
		/* overflow, refuse to do anything */
		ret = estrndup((char*)old, oldlen);
		retlen = oldlen;
		goto empty_source;
	}
	ret = emalloc(new_size);
	*ret = '\0';
	retlen = oldlen;
 	if (retlen == 0) {
 		goto empty_source;
 	}

 	inverse_map = unescape_inverse_map(all, flags);

 	/* replace numeric entities */
 	traverse_for_entities(old, oldlen, ret, &retlen, all, flags, inverse_map, charset);
 
empty_source:
 	*newlen = retlen;
 	return ret;
 }
