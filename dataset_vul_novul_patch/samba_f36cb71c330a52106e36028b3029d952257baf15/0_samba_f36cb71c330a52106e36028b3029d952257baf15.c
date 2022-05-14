static bool ldb_dn_explode(struct ldb_dn *dn)
{
	char *p, *ex_name = NULL, *ex_value = NULL, *data, *d, *dt, *t;
	bool trim = true;
	bool in_extended = true;
	bool in_ex_name = false;
	bool in_ex_value = false;
	bool in_attr = false;
	bool in_value = false;
	bool in_quote = false;
	bool is_oid = false;
	bool escape = false;
	unsigned int x;
	size_t l = 0;
	int ret;
	char *parse_dn;
	bool is_index;

	if ( ! dn || dn->invalid) return false;

	if (dn->components) {
		return true;
	}

	if (dn->ext_linearized) {
		parse_dn = dn->ext_linearized;
	} else {
		parse_dn = dn->linearized;
	}

	if ( ! parse_dn ) {
		return false;
	}

	is_index = (strncmp(parse_dn, "DN=@INDEX:", 10) == 0);

	/* Empty DNs */
	if (parse_dn[0] == '\0') {
		return true;
	}

	/* Special DNs case */
	if (dn->special) {
		return true;
	}

	/* make sure we free this if allocated previously before replacing */
	LDB_FREE(dn->components);
	dn->comp_num = 0;

	LDB_FREE(dn->ext_components);
	dn->ext_comp_num = 0;

	/* in the common case we have 3 or more components */
	/* make sure all components are zeroed, other functions depend on it */
	dn->components = talloc_zero_array(dn, struct ldb_dn_component, 3);
	if ( ! dn->components) {
		return false;
	}

	/* Components data space is allocated here once */
	data = talloc_array(dn->components, char, strlen(parse_dn) + 1);
	if (!data) {
		return false;
	}

	p = parse_dn;
	t = NULL;
	d = dt = data;

	while (*p) {
		if (in_extended) {

			if (!in_ex_name && !in_ex_value) {

				if (p[0] == '<') {
					p++;
					ex_name = d;
					in_ex_name = true;
					continue;
				} else if (p[0] == '\0') {
					p++;
					continue;
				} else {
					in_extended = false;
					in_attr = true;
					dt = d;

					continue;
				}
			}

			if (in_ex_name && *p == '=') {
				*d++ = '\0';
				p++;
				ex_value = d;
				in_ex_name = false;
				in_ex_value = true;
				continue;
			}

			if (in_ex_value && *p == '>') {
				const struct ldb_dn_extended_syntax *ext_syntax;
				struct ldb_val ex_val = {
					.data = (uint8_t *)ex_value,
					.length = d - ex_value
				};

				*d++ = '\0';
				p++;
				in_ex_value = false;

				/* Process name and ex_value */

				dn->ext_components = talloc_realloc(dn,
								    dn->ext_components,
								    struct ldb_dn_ext_component,
								    dn->ext_comp_num + 1);
				if ( ! dn->ext_components) {
					/* ouch ! */
					goto failed;
				}

				ext_syntax = ldb_dn_extended_syntax_by_name(dn->ldb, ex_name);
				if (!ext_syntax) {
					/* We don't know about this type of extended DN */
					goto failed;
				}

				dn->ext_components[dn->ext_comp_num].name = talloc_strdup(dn->ext_components, ex_name);
				if (!dn->ext_components[dn->ext_comp_num].name) {
					/* ouch */
					goto failed;
				}
				ret = ext_syntax->read_fn(dn->ldb, dn->ext_components,
							  &ex_val, &dn->ext_components[dn->ext_comp_num].value);
				if (ret != LDB_SUCCESS) {
					ldb_dn_mark_invalid(dn);
					goto failed;
				}

				dn->ext_comp_num++;

				if (*p == '\0') {
					/* We have reached the end (extended component only)! */
					talloc_free(data);
					return true;

				} else if (*p == ';') {
					p++;
					continue;
				} else {
					ldb_dn_mark_invalid(dn);
					goto failed;
				}
			}

			*d++ = *p++;
			continue;
		}
		if (in_attr) {
			if (trim) {
				if (*p == ' ') {
					p++;
					continue;
				}

				/* first char */
				trim = false;

				if (!isascii(*p)) {
					/* attr names must be ascii only */
					ldb_dn_mark_invalid(dn);
					goto failed;
				}

				if (isdigit(*p)) {
					is_oid = true;
				} else
				if ( ! isalpha(*p)) {
					/* not a digit nor an alpha,
 					 * invalid attribute name */
					ldb_dn_mark_invalid(dn);
					goto failed;
				}

				/* Copy this character across from parse_dn,
				 * now we have trimmed out spaces */
				*d++ = *p++;
				continue;
			}

			if (*p == ' ') {
				p++;
				/* valid only if we are at the end */
				trim = true;
				continue;
			}

			if (trim && (*p != '=')) {
				/* spaces/tabs are not allowed */
				ldb_dn_mark_invalid(dn);
				goto failed;
			}

			if (*p == '=') {
				/* attribute terminated */
				in_attr = false;
				in_value = true;
				trim = true;
				l = 0;

				/* Terminate this string in d
				 * (which is a copy of parse_dn
				 *  with spaces trimmed) */
				*d++ = '\0';
				dn->components[dn->comp_num].name = talloc_strdup(dn->components, dt);
				if ( ! dn->components[dn->comp_num].name) {
					/* ouch */
					goto failed;
				}

				dt = d;

				p++;
				continue;
			}

			if (!isascii(*p)) {
				/* attr names must be ascii only */
				ldb_dn_mark_invalid(dn);
				goto failed;
			}

			if (is_oid && ( ! (isdigit(*p) || (*p == '.')))) {
				/* not a digit nor a dot,
				 * invalid attribute oid */
				ldb_dn_mark_invalid(dn);
				goto failed;
			} else
			if ( ! (isalpha(*p) || isdigit(*p) || (*p == '-'))) {
				/* not ALPHA, DIGIT or HYPHEN */
				ldb_dn_mark_invalid(dn);
				goto failed;
			}

			*d++ = *p++;
			continue;
		}

		if (in_value) {
			if (in_quote) {
				if (*p == '\"') {
					if (p[-1] != '\\') {
						p++;
						in_quote = false;
						continue;
					}
				}
				*d++ = *p++;
				l++;
				continue;
			}

			if (trim) {
				if (*p == ' ') {
					p++;
					continue;
				}

				/* first char */
				trim = false;

				if (*p == '\"') {
					in_quote = true;
					p++;
					continue;
				}
			}

			switch (*p) {

			/* TODO: support ber encoded values
			case '#':
			*/

			case ',':
				if (escape) {
					*d++ = *p++;
					l++;
					escape = false;
					continue;
				}
				/* ok found value terminator */

				if ( t ) {
					/* trim back */
					d -= (p - t);
					l -= (p - t);
				}

				in_attr = true;
				in_value = false;
				trim = true;
 
                                p++;
                                *d++ = '\0';
                               dn->components[dn->comp_num].value.data = \
                                       (uint8_t *)talloc_memdup(dn->components, dt, l + 1);
                                dn->components[dn->comp_num].value.length = l;
                                if ( ! dn->components[dn->comp_num].value.data) {
                                        /* ouch ! */
                                        goto failed;
                                }
                               talloc_set_name_const(dn->components[dn->comp_num].value.data,
                                                     (const char *)dn->components[dn->comp_num].value.data);
 
                                dt = d;
 
									dn->components,
									struct ldb_dn_component,
									dn->comp_num + 1);
					if ( ! dn->components) {
						/* ouch ! */
						goto failed;
					}
					/* make sure all components are zeroed, other functions depend on this */
					memset(&dn->components[dn->comp_num], '\0', sizeof(struct ldb_dn_component));
				}

				continue;

			case '+':
			case '=':
				/* to main compatibility with earlier
				versions of ldb indexing, we have to
				accept the base64 encoded binary index
				values, which contain a '+' or '='
				which should normally be escaped */
				if (is_index) {
					if ( t ) t = NULL;
					*d++ = *p++;
					l++;
					break;
				}
				/* fall through */
			case '\"':
			case '<':
			case '>':
			case ';':
				/* a string with not escaped specials is invalid (tested) */
				if ( ! escape) {
					ldb_dn_mark_invalid(dn);
					goto failed;
				}
				escape = false;

				*d++ = *p++;
				l++;

				if ( t ) t = NULL;
				break;

			case '\\':
				if ( ! escape) {
					escape = true;
					p++;
					continue;
				}
				escape = false;

				*d++ = *p++;
				l++;

				if ( t ) t = NULL;
				break;

			default:
				if (escape) {
					if (isxdigit(p[0]) && isxdigit(p[1])) {
						if (sscanf(p, "%02x", &x) != 1) {
							/* invalid escaping sequence */
							ldb_dn_mark_invalid(dn);
							goto failed;
						}
						p += 2;
						*d++ = (unsigned char)x;
					} else {
						*d++ = *p++;
					}

					escape = false;
					l++;
					if ( t ) t = NULL;
					break;
				}

				if (*p == ' ') {
					if ( ! t) t = p;
				} else {
					if ( t ) t = NULL;
				}

				*d++ = *p++;
				l++;

				break;
			}

		}
	}
