struct json_object* json_tokener_parse_ex(struct json_tokener *tok,
					  const char *str, int len)
{
  struct json_object *obj = NULL;
  char c = '\1';
#ifdef HAVE_SETLOCALE
  char *oldlocale=NULL, *tmplocale;

  tmplocale = setlocale(LC_NUMERIC, NULL);
  if (tmplocale) oldlocale = strdup(tmplocale);
  setlocale(LC_NUMERIC, "C");
#endif

   tok->char_offset = 0;
   tok->err = json_tokener_success;
 
   while (PEEK_CHAR(c, tok)) {
 
   redo_char:
    switch(state) {

    case json_tokener_state_eatws:
      /* Advance until we change state */
      while (isspace((int)c)) {
	if ((!ADVANCE_CHAR(str, tok)) || (!PEEK_CHAR(c, tok)))
	  goto out;
      }
      if(c == '/' && !(tok->flags & JSON_TOKENER_STRICT)) {
	printbuf_reset(tok->pb);
	printbuf_memappend_fast(tok->pb, &c, 1);
	state = json_tokener_state_comment_start;
      } else {
	state = saved_state;
	goto redo_char;
      }
      break;

    case json_tokener_state_start:
      switch(c) {
      case '{':
	state = json_tokener_state_eatws;
	saved_state = json_tokener_state_object_field_start;
	current = json_object_new_object();
	break;
      case '[':
	state = json_tokener_state_eatws;
	saved_state = json_tokener_state_array;
	current = json_object_new_array();
	break;
      case 'I':
      case 'i':
	state = json_tokener_state_inf;
	printbuf_reset(tok->pb);
	tok->st_pos = 0;
	goto redo_char;
      case 'N':
      case 'n':
	state = json_tokener_state_null; // or NaN
	printbuf_reset(tok->pb);
	tok->st_pos = 0;
	goto redo_char;
      case '\'':
        if (tok->flags & JSON_TOKENER_STRICT) {
            /* in STRICT mode only double-quote are allowed */
            tok->err = json_tokener_error_parse_unexpected;
            goto out;
        }
      case '"':
	state = json_tokener_state_string;
	printbuf_reset(tok->pb);
	tok->quote_char = c;
	break;
      case 'T':
      case 't':
      case 'F':
      case 'f':
	state = json_tokener_state_boolean;
	printbuf_reset(tok->pb);
	tok->st_pos = 0;
	goto redo_char;
#if defined(__GNUC__)
	  case '0' ... '9':
#else
	  case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
#endif
      case '-':
	state = json_tokener_state_number;
	printbuf_reset(tok->pb);
	tok->is_double = 0;
	goto redo_char;
      default:
	tok->err = json_tokener_error_parse_unexpected;
	goto out;
      }
      break;

    case json_tokener_state_finish:
      if(tok->depth == 0) goto out;
      obj = json_object_get(current);
      json_tokener_reset_level(tok, tok->depth);
      tok->depth--;
      goto redo_char;

    case json_tokener_state_inf: /* aka starts with 'i' */
      {
	int size;
	int size_inf;
	int is_negative = 0;

	printbuf_memappend_fast(tok->pb, &c, 1);
	size = json_min(tok->st_pos+1, json_null_str_len);
	size_inf = json_min(tok->st_pos+1, json_inf_str_len);
	char *infbuf = tok->pb->buf;
	if (*infbuf == '-')
	{
		infbuf++;
		is_negative = 1;
	}
	if ((!(tok->flags & JSON_TOKENER_STRICT) &&
	          strncasecmp(json_inf_str, infbuf, size_inf) == 0) ||
	         (strncmp(json_inf_str, infbuf, size_inf) == 0)
	        )
	{
		if (tok->st_pos == json_inf_str_len)
		{
			current = json_object_new_double(is_negative ? -INFINITY : INFINITY); 
			saved_state = json_tokener_state_finish;
			state = json_tokener_state_eatws;
			goto redo_char;
		}
	} else {
		tok->err = json_tokener_error_parse_unexpected;
		goto out;
	}
	tok->st_pos++;
      }
      break;
    case json_tokener_state_null: /* aka starts with 'n' */
      {
	int size;
	int size_nan;
	printbuf_memappend_fast(tok->pb, &c, 1);
	size = json_min(tok->st_pos+1, json_null_str_len);
	size_nan = json_min(tok->st_pos+1, json_nan_str_len);
	if((!(tok->flags & JSON_TOKENER_STRICT) &&
	  strncasecmp(json_null_str, tok->pb->buf, size) == 0)
	  || (strncmp(json_null_str, tok->pb->buf, size) == 0)
	  ) {
	  if (tok->st_pos == json_null_str_len) {
	    current = NULL;
	    saved_state = json_tokener_state_finish;
	    state = json_tokener_state_eatws;
	    goto redo_char;
	  }
	}
	else if ((!(tok->flags & JSON_TOKENER_STRICT) &&
	          strncasecmp(json_nan_str, tok->pb->buf, size_nan) == 0) ||
	         (strncmp(json_nan_str, tok->pb->buf, size_nan) == 0)
	        )
	{
		if (tok->st_pos == json_nan_str_len)
		{
			current = json_object_new_double(NAN);
			saved_state = json_tokener_state_finish;
			state = json_tokener_state_eatws;
			goto redo_char;
		}
	} else {
	  tok->err = json_tokener_error_parse_null;
	  goto out;
	}
	tok->st_pos++;
      }
      break;

    case json_tokener_state_comment_start:
      if(c == '*') {
	state = json_tokener_state_comment;
      } else if(c == '/') {
	state = json_tokener_state_comment_eol;
      } else {
	tok->err = json_tokener_error_parse_comment;
	goto out;
      }
      printbuf_memappend_fast(tok->pb, &c, 1);
      break;

    case json_tokener_state_comment:
              {
          /* Advance until we change state */
          const char *case_start = str;
          while(c != '*') {
            if (!ADVANCE_CHAR(str, tok) || !PEEK_CHAR(c, tok)) {
              printbuf_memappend_fast(tok->pb, case_start, str-case_start);
              goto out;
            }
          }
          printbuf_memappend_fast(tok->pb, case_start, 1+str-case_start);
          state = json_tokener_state_comment_end;
        }
            break;

    case json_tokener_state_comment_eol:
      {
	/* Advance until we change state */
	const char *case_start = str;
	while(c != '\n') {
	  if (!ADVANCE_CHAR(str, tok) || !PEEK_CHAR(c, tok)) {
	    printbuf_memappend_fast(tok->pb, case_start, str-case_start);
	    goto out;
	  }
	}
	printbuf_memappend_fast(tok->pb, case_start, str-case_start);
	MC_DEBUG("json_tokener_comment: %s\n", tok->pb->buf);
	state = json_tokener_state_eatws;
      }
      break;

    case json_tokener_state_comment_end:
      printbuf_memappend_fast(tok->pb, &c, 1);
      if(c == '/') {
	MC_DEBUG("json_tokener_comment: %s\n", tok->pb->buf);
	state = json_tokener_state_eatws;
      } else {
	state = json_tokener_state_comment;
      }
      break;

    case json_tokener_state_string:
      {
	/* Advance until we change state */
	const char *case_start = str;
	while(1) {
	  if(c == tok->quote_char) {
	    printbuf_memappend_fast(tok->pb, case_start, str-case_start);
	    current = json_object_new_string_len(tok->pb->buf, tok->pb->bpos);
	    saved_state = json_tokener_state_finish;
	    state = json_tokener_state_eatws;
	    break;
	  } else if(c == '\\') {
	    printbuf_memappend_fast(tok->pb, case_start, str-case_start);
	    saved_state = json_tokener_state_string;
	    state = json_tokener_state_string_escape;
	    break;
	  }
	  if (!ADVANCE_CHAR(str, tok) || !PEEK_CHAR(c, tok)) {
	    printbuf_memappend_fast(tok->pb, case_start, str-case_start);
	    goto out;
	  }
	}
      }
      break;

    case json_tokener_state_string_escape:
      switch(c) {
      case '"':
      case '\\':
      case '/':
	printbuf_memappend_fast(tok->pb, &c, 1);
	state = saved_state;
	break;
      case 'b':
      case 'n':
      case 'r':
      case 't':
      case 'f':
	if(c == 'b') printbuf_memappend_fast(tok->pb, "\b", 1);
	else if(c == 'n') printbuf_memappend_fast(tok->pb, "\n", 1);
	else if(c == 'r') printbuf_memappend_fast(tok->pb, "\r", 1);
	else if(c == 't') printbuf_memappend_fast(tok->pb, "\t", 1);
	else if(c == 'f') printbuf_memappend_fast(tok->pb, "\f", 1);
	state = saved_state;
	break;
      case 'u':
	tok->ucs_char = 0;
	tok->st_pos = 0;
	state = json_tokener_state_escape_unicode;
	break;
      default:
	tok->err = json_tokener_error_parse_string;
	goto out;
      }
      break;

    case json_tokener_state_escape_unicode:
	{
          unsigned int got_hi_surrogate = 0;

	  /* Handle a 4-byte sequence, or two sequences if a surrogate pair */
	  while(1) {
	    if(strchr(json_hex_chars, c)) {
	      tok->ucs_char += ((unsigned int)hexdigit(c) << ((3-tok->st_pos++)*4));
	      if(tok->st_pos == 4) {
		unsigned char unescaped_utf[4];

                if (got_hi_surrogate) {
		  if (IS_LOW_SURROGATE(tok->ucs_char)) {
                    /* Recalculate the ucs_char, then fall thru to process normally */
                    tok->ucs_char = DECODE_SURROGATE_PAIR(got_hi_surrogate, tok->ucs_char);
                  } else {
                    /* Hi surrogate was not followed by a low surrogate */
                    /* Replace the hi and process the rest normally */
		    printbuf_memappend_fast(tok->pb, (char*)utf8_replacement_char, 3);
                  }
                  got_hi_surrogate = 0;
                }

		if (tok->ucs_char < 0x80) {
		  unescaped_utf[0] = tok->ucs_char;
		  printbuf_memappend_fast(tok->pb, (char*)unescaped_utf, 1);
		} else if (tok->ucs_char < 0x800) {
		  unescaped_utf[0] = 0xc0 | (tok->ucs_char >> 6);
		  unescaped_utf[1] = 0x80 | (tok->ucs_char & 0x3f);
		  printbuf_memappend_fast(tok->pb, (char*)unescaped_utf, 2);
		} else if (IS_HIGH_SURROGATE(tok->ucs_char)) {
                  /* Got a high surrogate.  Remember it and look for the
                   * the beginning of another sequence, which should be the
                   * low surrogate.
                   */
                  got_hi_surrogate = tok->ucs_char;
                  /* Not at end, and the next two chars should be "\u" */
                  if ((tok->char_offset+1 != len) &&
                      (tok->char_offset+2 != len) &&
                      (str[1] == '\\') &&
                      (str[2] == 'u'))
                  {
                /* Advance through the 16 bit surrogate, and move on to the
                 * next sequence. The next step is to process the following
                 * characters.
                 */
	            if( !ADVANCE_CHAR(str, tok) || !ADVANCE_CHAR(str, tok) ) {
                    printbuf_memappend_fast(tok->pb, (char*)utf8_replacement_char, 3);
                }
                    /* Advance to the first char of the next sequence and
                     * continue processing with the next sequence.
                     */
	            if (!ADVANCE_CHAR(str, tok) || !PEEK_CHAR(c, tok)) {
	              printbuf_memappend_fast(tok->pb, (char*)utf8_replacement_char, 3);
	              goto out;
                    }
	            tok->ucs_char = 0;
                    tok->st_pos = 0;
                    continue; /* other json_tokener_state_escape_unicode */
                  } else {
                    /* Got a high surrogate without another sequence following
                     * it.  Put a replacement char in for the hi surrogate
                     * and pretend we finished.
                     */
		    printbuf_memappend_fast(tok->pb, (char*)utf8_replacement_char, 3);
                  }
		} else if (IS_LOW_SURROGATE(tok->ucs_char)) {
                  /* Got a low surrogate not preceded by a high */
		  printbuf_memappend_fast(tok->pb, (char*)utf8_replacement_char, 3);
                } else if (tok->ucs_char < 0x10000) {
		  unescaped_utf[0] = 0xe0 | (tok->ucs_char >> 12);
		  unescaped_utf[1] = 0x80 | ((tok->ucs_char >> 6) & 0x3f);
		  unescaped_utf[2] = 0x80 | (tok->ucs_char & 0x3f);
		  printbuf_memappend_fast(tok->pb, (char*)unescaped_utf, 3);
		} else if (tok->ucs_char < 0x110000) {
		  unescaped_utf[0] = 0xf0 | ((tok->ucs_char >> 18) & 0x07);
		  unescaped_utf[1] = 0x80 | ((tok->ucs_char >> 12) & 0x3f);
		  unescaped_utf[2] = 0x80 | ((tok->ucs_char >> 6) & 0x3f);
		  unescaped_utf[3] = 0x80 | (tok->ucs_char & 0x3f);
		  printbuf_memappend_fast(tok->pb, (char*)unescaped_utf, 4);
		} else {
                  /* Don't know what we got--insert the replacement char */
		  printbuf_memappend_fast(tok->pb, (char*)utf8_replacement_char, 3);
                }
		state = saved_state;
		break;
	      }
	    } else {
	      tok->err = json_tokener_error_parse_string;
	      goto out;
	    }
	  if (!ADVANCE_CHAR(str, tok) || !PEEK_CHAR(c, tok)) {
            if (got_hi_surrogate) /* Clean up any pending chars */
	      printbuf_memappend_fast(tok->pb, (char*)utf8_replacement_char, 3);
	    goto out;
	  }
	}
      }
      break;

    case json_tokener_state_boolean:
      {
	int size1, size2;
	printbuf_memappend_fast(tok->pb, &c, 1);
	size1 = json_min(tok->st_pos+1, json_true_str_len);
	size2 = json_min(tok->st_pos+1, json_false_str_len);
	if((!(tok->flags & JSON_TOKENER_STRICT) &&
	  strncasecmp(json_true_str, tok->pb->buf, size1) == 0)
	  || (strncmp(json_true_str, tok->pb->buf, size1) == 0)
	  ) {
	  if(tok->st_pos == json_true_str_len) {
	    current = json_object_new_boolean(1);
	    saved_state = json_tokener_state_finish;
	    state = json_tokener_state_eatws;
	    goto redo_char;
	  }
	} else if((!(tok->flags & JSON_TOKENER_STRICT) &&
	  strncasecmp(json_false_str, tok->pb->buf, size2) == 0)
	  || (strncmp(json_false_str, tok->pb->buf, size2) == 0)) {
	  if(tok->st_pos == json_false_str_len) {
	    current = json_object_new_boolean(0);
	    saved_state = json_tokener_state_finish;
	    state = json_tokener_state_eatws;
	    goto redo_char;
	  }
	} else {
	  tok->err = json_tokener_error_parse_boolean;
	  goto out;
	}
	tok->st_pos++;
      }
      break;

    case json_tokener_state_number:
      {
	/* Advance until we change state */
	const char *case_start = str;
	int case_len=0;
	while(c && strchr(json_number_chars, c)) {
	  ++case_len;
	  if(c == '.' || c == 'e' || c == 'E')
	    tok->is_double = 1;
	  if (!ADVANCE_CHAR(str, tok) || !PEEK_CHAR(c, tok)) {
	    printbuf_memappend_fast(tok->pb, case_start, case_len);
	    goto out;
	  }
	}
        if (case_len>0)
          printbuf_memappend_fast(tok->pb, case_start, case_len);

	if (tok->pb->buf[0] == '-' && case_len == 1 &&
	    (c == 'i' || c == 'I'))
	{
		state = json_tokener_state_inf;
		goto redo_char;
	}
      }
      {
	int64_t num64;
	double  numd;
	if (!tok->is_double && json_parse_int64(tok->pb->buf, &num64) == 0) {
		if (num64 && tok->pb->buf[0]=='0' && (tok->flags & JSON_TOKENER_STRICT)) {
			/* in strict mode, number must not start with 0 */
			tok->err = json_tokener_error_parse_number;
			goto out;
		}
		current = json_object_new_int64(num64);
	}
	else if(tok->is_double && json_parse_double(tok->pb->buf, &numd) == 0)
	{
          current = json_object_new_double_s(numd, tok->pb->buf);
        } else {
          tok->err = json_tokener_error_parse_number;
          goto out;
        }
        saved_state = json_tokener_state_finish;
        state = json_tokener_state_eatws;
        goto redo_char;
      }
      break;

    case json_tokener_state_array_after_sep:
    case json_tokener_state_array:
      if(c == ']') {
		if (state == json_tokener_state_array_after_sep &&
			(tok->flags & JSON_TOKENER_STRICT))
		{
			tok->err = json_tokener_error_parse_unexpected;
			goto out;
		}
	saved_state = json_tokener_state_finish;
	state = json_tokener_state_eatws;
      } else {
	if(tok->depth >= tok->max_depth-1) {
	  tok->err = json_tokener_error_depth;
	  goto out;
	}
	state = json_tokener_state_array_add;
	tok->depth++;
	json_tokener_reset_level(tok, tok->depth);
	goto redo_char;
      }
      break;

    case json_tokener_state_array_add:
      json_object_array_add(current, obj);
      saved_state = json_tokener_state_array_sep;
      state = json_tokener_state_eatws;
      goto redo_char;

    case json_tokener_state_array_sep:
      if(c == ']') {
	saved_state = json_tokener_state_finish;
	state = json_tokener_state_eatws;
      } else if(c == ',') {
	saved_state = json_tokener_state_array_after_sep;
	state = json_tokener_state_eatws;
      } else {
	tok->err = json_tokener_error_parse_array;
	goto out;
      }
      break;

    case json_tokener_state_object_field_start:
    case json_tokener_state_object_field_start_after_sep:
      if(c == '}') {
		if (state == json_tokener_state_object_field_start_after_sep &&
		    (tok->flags & JSON_TOKENER_STRICT))
		{
			tok->err = json_tokener_error_parse_unexpected;
			goto out;
		}
	saved_state = json_tokener_state_finish;
	state = json_tokener_state_eatws;
      } else if (c == '"' || c == '\'') {
	tok->quote_char = c;
	printbuf_reset(tok->pb);
	state = json_tokener_state_object_field;
      } else {
	tok->err = json_tokener_error_parse_object_key_name;
	goto out;
      }
      break;

    case json_tokener_state_object_field:
      {
	/* Advance until we change state */
	const char *case_start = str;
	while(1) {
	  if(c == tok->quote_char) {
	    printbuf_memappend_fast(tok->pb, case_start, str-case_start);
	    obj_field_name = strdup(tok->pb->buf);
	    saved_state = json_tokener_state_object_field_end;
	    state = json_tokener_state_eatws;
	    break;
	  } else if(c == '\\') {
	    printbuf_memappend_fast(tok->pb, case_start, str-case_start);
	    saved_state = json_tokener_state_object_field;
	    state = json_tokener_state_string_escape;
	    break;
	  }
	  if (!ADVANCE_CHAR(str, tok) || !PEEK_CHAR(c, tok)) {
	    printbuf_memappend_fast(tok->pb, case_start, str-case_start);
	    goto out;
	  }
	}
      }
      break;

    case json_tokener_state_object_field_end:
      if(c == ':') {
	saved_state = json_tokener_state_object_value;
	state = json_tokener_state_eatws;
      } else {
	tok->err = json_tokener_error_parse_object_key_sep;
	goto out;
      }
      break;

    case json_tokener_state_object_value:
      if(tok->depth >= tok->max_depth-1) {
	tok->err = json_tokener_error_depth;
	goto out;
      }
      state = json_tokener_state_object_value_add;
      tok->depth++;
      json_tokener_reset_level(tok, tok->depth);
      goto redo_char;

    case json_tokener_state_object_value_add:
      json_object_object_add(current, obj_field_name, obj);
      free(obj_field_name);
      obj_field_name = NULL;
      saved_state = json_tokener_state_object_sep;
      state = json_tokener_state_eatws;
      goto redo_char;

    case json_tokener_state_object_sep:
      if(c == '}') {
	saved_state = json_tokener_state_finish;
	state = json_tokener_state_eatws;
      } else if(c == ',') {
	saved_state = json_tokener_state_object_field_start_after_sep;
	state = json_tokener_state_eatws;
      } else {
	tok->err = json_tokener_error_parse_object_value_sep;
	goto out;
      }
      break;

    }
    if (!ADVANCE_CHAR(str, tok))
      goto out;
  } /* while(POP_CHAR) */
