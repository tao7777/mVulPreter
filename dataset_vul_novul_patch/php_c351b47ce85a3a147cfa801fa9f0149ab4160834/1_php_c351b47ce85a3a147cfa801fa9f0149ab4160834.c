PHPAPI void php_pcre_match_impl(pcre_cache_entry *pce, char *subject, int subject_len, zval *return_value,
	zval *subpats, int global, int use_flags, long flags, long start_offset TSRMLS_DC)
{
	zval			*result_set,		/* Holds a set of subpatterns after
										   a global match */
				   **match_sets = NULL;	/* An array of sets of matches for each
										   subpattern after a global match */
	pcre_extra		*extra = pce->extra;/* Holds results of studying */
	pcre_extra		 extra_data;		/* Used locally for exec options */
	int				 exoptions = 0;		/* Execution options */
	int				 count = 0;			/* Count of matched subpatterns */
	int				*offsets;			/* Array of subpattern offsets */
	int				 num_subpats;		/* Number of captured subpatterns */
	int				 size_offsets;		/* Size of the offsets array */
	int				 matched;			/* Has anything matched */
	int				 g_notempty = 0;	/* If the match should not be empty */
	const char	   **stringlist;		/* Holds list of subpatterns */
	char 		   **subpat_names;		/* Array for named subpatterns */
	int				 i, rc;
	int				 subpats_order;		/* Order of subpattern matches */
	int				 offset_capture;    /* Capture match offsets: yes/no */

	/* Overwrite the passed-in value for subpatterns with an empty array. */
	if (subpats != NULL) {
		zval_dtor(subpats);
		array_init(subpats);
	}

	subpats_order = global ? PREG_PATTERN_ORDER : 0;

	if (use_flags) {
		offset_capture = flags & PREG_OFFSET_CAPTURE;

		/*
		 * subpats_order is pre-set to pattern mode so we change it only if
		 * necessary.
		 */
		if (flags & 0xff) {
			subpats_order = flags & 0xff;
		}
		if ((global && (subpats_order < PREG_PATTERN_ORDER || subpats_order > PREG_SET_ORDER)) ||
			(!global && subpats_order != 0)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid flags specified");
			return;
		}
	} else {
		offset_capture = 0;
	}

	/* Negative offset counts from the end of the string. */
	if (start_offset < 0) {
		start_offset = subject_len + start_offset;
		if (start_offset < 0) {
			start_offset = 0;
		}
	}

	if (extra == NULL) {
		extra_data.flags = PCRE_EXTRA_MATCH_LIMIT | PCRE_EXTRA_MATCH_LIMIT_RECURSION;
		extra = &extra_data;
	}
	extra->match_limit = PCRE_G(backtrack_limit);
	extra->match_limit_recursion = PCRE_G(recursion_limit);

	/* Calculate the size of the offsets array, and allocate memory for it. */
	rc = pcre_fullinfo(pce->re, extra, PCRE_INFO_CAPTURECOUNT, &num_subpats);
	if (rc < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Internal pcre_fullinfo() error %d", rc);
		RETURN_FALSE;
	}
	num_subpats++;
	size_offsets = num_subpats * 3;

	/*
	 * Build a mapping from subpattern numbers to their names. We will always
	 * allocate the table, even though there may be no named subpatterns. This
	 * avoids somewhat more complicated logic in the inner loops.
	 */
	subpat_names = make_subpats_table(num_subpats, pce TSRMLS_CC);
	if (!subpat_names) {
		RETURN_FALSE;
        }
 
        offsets = (int *)safe_emalloc(size_offsets, sizeof(int), 0);
        /* Allocate match sets array and initialize the values. */
        if (global && subpats && subpats_order == PREG_PATTERN_ORDER) {
                match_sets = (zval **)safe_emalloc(num_subpats, sizeof(zval *), 0);
		for (i=0; i<num_subpats; i++) {
			ALLOC_ZVAL(match_sets[i]);
			array_init(match_sets[i]);
			INIT_PZVAL(match_sets[i]);
		}
	}

	matched = 0;
	PCRE_G(error_code) = PHP_PCRE_NO_ERROR;
	
	do {
		/* Execute the regular expression. */
		count = pcre_exec(pce->re, extra, subject, subject_len, start_offset,
						  exoptions|g_notempty, offsets, size_offsets);

		/* the string was already proved to be valid UTF-8 */
		exoptions |= PCRE_NO_UTF8_CHECK;

		/* Check for too many substrings condition. */
		if (count == 0) {
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Matched, but too many substrings");
			count = size_offsets/3;
		}

		/* If something has matched */
		if (count > 0) {
			matched++;

			/* If subpatterns array has been passed, fill it in with values. */
			if (subpats != NULL) {
				/* Try to get the list of substrings and display a warning if failed. */
				if (pcre_get_substring_list(subject, offsets, count, &stringlist) < 0) {
					efree(subpat_names);
					efree(offsets);
					if (match_sets) efree(match_sets);
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Get subpatterns list failed");
					RETURN_FALSE;
				}

				if (global) {	/* global pattern matching */
					if (subpats && subpats_order == PREG_PATTERN_ORDER) {
						/* For each subpattern, insert it into the appropriate array. */
						for (i = 0; i < count; i++) {
							if (offset_capture) {
								add_offset_pair(match_sets[i], (char *)stringlist[i],
												offsets[(i<<1)+1] - offsets[i<<1], offsets[i<<1], NULL);
							} else {
								add_next_index_stringl(match_sets[i], (char *)stringlist[i],
													   offsets[(i<<1)+1] - offsets[i<<1], 1);
							}
						}
						/*
						 * If the number of captured subpatterns on this run is
						 * less than the total possible number, pad the result
						 * arrays with empty strings.
						 */
						if (count < num_subpats) {
							for (; i < num_subpats; i++) {
								add_next_index_string(match_sets[i], "", 1);
							}
						}
					} else {
						/* Allocate the result set array */
						ALLOC_ZVAL(result_set);
						array_init(result_set);
						INIT_PZVAL(result_set);
						
						/* Add all the subpatterns to it */
						for (i = 0; i < count; i++) {
							if (offset_capture) {
								add_offset_pair(result_set, (char *)stringlist[i],
												offsets[(i<<1)+1] - offsets[i<<1], offsets[i<<1], subpat_names[i]);
							} else {
								if (subpat_names[i]) {
									add_assoc_stringl(result_set, subpat_names[i], (char *)stringlist[i],
														   offsets[(i<<1)+1] - offsets[i<<1], 1);
								}
								add_next_index_stringl(result_set, (char *)stringlist[i],
													   offsets[(i<<1)+1] - offsets[i<<1], 1);
							}
						}
						/* And add it to the output array */
						zend_hash_next_index_insert(Z_ARRVAL_P(subpats), &result_set, sizeof(zval *), NULL);
					}
				} else {			/* single pattern matching */
					/* For each subpattern, insert it into the subpatterns array. */
					for (i = 0; i < count; i++) {
						if (offset_capture) {
							add_offset_pair(subpats, (char *)stringlist[i],
											offsets[(i<<1)+1] - offsets[i<<1],
											offsets[i<<1], subpat_names[i]);
						} else {
							if (subpat_names[i]) {
								add_assoc_stringl(subpats, subpat_names[i], (char *)stringlist[i],
												  offsets[(i<<1)+1] - offsets[i<<1], 1);
							}
							add_next_index_stringl(subpats, (char *)stringlist[i],
												   offsets[(i<<1)+1] - offsets[i<<1], 1);
						}
					}
				}

				pcre_free((void *) stringlist);
			}
		} else if (count == PCRE_ERROR_NOMATCH) {
			/* If we previously set PCRE_NOTEMPTY after a null match,
			   this is not necessarily the end. We need to advance
			   the start offset, and continue. Fudge the offset values
			   to achieve this, unless we're already at the end of the string. */
			if (g_notempty != 0 && start_offset < subject_len) {
				offsets[0] = start_offset;
				offsets[1] = start_offset + 1;
			} else
				break;
		} else {
			pcre_handle_exec_error(count TSRMLS_CC);
			break;
		}
		
		/* If we have matched an empty string, mimic what Perl's /g options does.
		   This turns out to be rather cunning. First we set PCRE_NOTEMPTY and try
		   the match again at the same point. If this fails (picked up above) we
		   advance to the next character. */
		g_notempty = (offsets[1] == offsets[0])? PCRE_NOTEMPTY | PCRE_ANCHORED : 0;
		
		/* Advance to the position right after the last full match */
		start_offset = offsets[1];
	} while (global);

	/* Add the match sets to the output array and clean up */
	if (global && subpats && subpats_order == PREG_PATTERN_ORDER) {
		for (i = 0; i < num_subpats; i++) {
			if (subpat_names[i]) {
				zend_hash_update(Z_ARRVAL_P(subpats), subpat_names[i],
								 strlen(subpat_names[i])+1, &match_sets[i], sizeof(zval *), NULL);
				Z_ADDREF_P(match_sets[i]);
			}
			zend_hash_next_index_insert(Z_ARRVAL_P(subpats), &match_sets[i], sizeof(zval *), NULL);
		}
		efree(match_sets);
	}
	
	efree(offsets);
	efree(subpat_names);

	/* Did we encounter an error? */
	if (PCRE_G(error_code) == PHP_PCRE_NO_ERROR) {
		RETVAL_LONG(matched);
	} else {
		RETVAL_FALSE;
	}
}
