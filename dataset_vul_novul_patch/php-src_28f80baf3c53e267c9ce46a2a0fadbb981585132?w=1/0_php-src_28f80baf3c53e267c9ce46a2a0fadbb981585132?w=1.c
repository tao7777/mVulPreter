php_mysqlnd_rowp_read_text_protocol_aux(MYSQLND_MEMORY_POOL_CHUNK * row_buffer, zval ** fields,
									unsigned int field_count, const MYSQLND_FIELD * fields_metadata,
									zend_bool as_int_or_float, zend_bool copy_data, MYSQLND_STATS * stats TSRMLS_DC)
{
	
	unsigned int i;
	zend_bool last_field_was_string = FALSE;
	zval **current_field, **end_field, **start_field;
 	zend_uchar * p = row_buffer->ptr;
 	size_t data_size = row_buffer->app;
 	zend_uchar * bit_area = (zend_uchar*) row_buffer->ptr + data_size + 1; /* we allocate from here */
	const zend_uchar * const packet_end = (zend_uchar*) row_buffer->ptr + data_size;
 
 	DBG_ENTER("php_mysqlnd_rowp_read_text_protocol_aux");
 
	if (!fields) {
		DBG_RETURN(FAIL);
	}

	end_field = (start_field = fields) + field_count;

	for (i = 0, current_field = start_field; current_field < end_field; current_field++, i++) {
		DBG_INF("Directly creating zval");
		MAKE_STD_ZVAL(*current_field);
		if (!*current_field) {
			DBG_RETURN(FAIL);
		}
	}

	for (i = 0, current_field = start_field; current_field < end_field; current_field++, i++) {
 		/* Don't reverse the order. It is significant!*/
 		zend_uchar *this_field_len_pos = p;
 		/* php_mysqlnd_net_field_length() call should be after *this_field_len_pos = p; */
		const unsigned long len = php_mysqlnd_net_field_length(&p);
 
		if (len != MYSQLND_NULL_LENGTH && ((p + len) > packet_end)) {
			php_error_docref(NULL, E_WARNING, "Malformed server packet. Field length pointing "MYSQLND_SZ_T_SPEC
											  " bytes after end of packet", (p + len) - packet_end - 1);
			DBG_RETURN(FAIL);
		}
 		if (copy_data == FALSE && current_field > start_field && last_field_was_string) {
 			/*
 			  Normal queries:
			  We have to put \0 now to the end of the previous field, if it was
			  a string. IS_NULL doesn't matter. Because we have already read our
			  length, then we can overwrite it in the row buffer.
			  This statement terminates the previous field, not the current one.

			  NULL_LENGTH is encoded in one byte, so we can stick a \0 there.
			  Any string's length is encoded in at least one byte, so we can stick
			  a \0 there.
			*/

			*this_field_len_pos = '\0';
		}

		/* NULL or NOT NULL, this is the question! */
		if (len == MYSQLND_NULL_LENGTH) {
			ZVAL_NULL(*current_field);
			last_field_was_string = FALSE;
		} else {
#if defined(MYSQLND_STRING_TO_INT_CONVERSION)
			struct st_mysqlnd_perm_bind perm_bind =
					mysqlnd_ps_fetch_functions[fields_metadata[i].type];
#endif
			if (MYSQLND_G(collect_statistics)) {
				enum_mysqlnd_collected_stats statistic;
				switch (fields_metadata[i].type) {
					case MYSQL_TYPE_DECIMAL:	statistic = STAT_TEXT_TYPE_FETCHED_DECIMAL; break;
					case MYSQL_TYPE_TINY:		statistic = STAT_TEXT_TYPE_FETCHED_INT8; break;
					case MYSQL_TYPE_SHORT:		statistic = STAT_TEXT_TYPE_FETCHED_INT16; break;
					case MYSQL_TYPE_LONG:		statistic = STAT_TEXT_TYPE_FETCHED_INT32; break;
					case MYSQL_TYPE_FLOAT:		statistic = STAT_TEXT_TYPE_FETCHED_FLOAT; break;
					case MYSQL_TYPE_DOUBLE:		statistic = STAT_TEXT_TYPE_FETCHED_DOUBLE; break;
					case MYSQL_TYPE_NULL:		statistic = STAT_TEXT_TYPE_FETCHED_NULL; break;
					case MYSQL_TYPE_TIMESTAMP:	statistic = STAT_TEXT_TYPE_FETCHED_TIMESTAMP; break;
					case MYSQL_TYPE_LONGLONG:	statistic = STAT_TEXT_TYPE_FETCHED_INT64; break;
					case MYSQL_TYPE_INT24:		statistic = STAT_TEXT_TYPE_FETCHED_INT24; break;
					case MYSQL_TYPE_DATE:		statistic = STAT_TEXT_TYPE_FETCHED_DATE; break;
					case MYSQL_TYPE_TIME:		statistic = STAT_TEXT_TYPE_FETCHED_TIME; break;
					case MYSQL_TYPE_DATETIME:	statistic = STAT_TEXT_TYPE_FETCHED_DATETIME; break;
					case MYSQL_TYPE_YEAR:		statistic = STAT_TEXT_TYPE_FETCHED_YEAR; break;
					case MYSQL_TYPE_NEWDATE:	statistic = STAT_TEXT_TYPE_FETCHED_DATE; break;
					case MYSQL_TYPE_VARCHAR:	statistic = STAT_TEXT_TYPE_FETCHED_STRING; break;
					case MYSQL_TYPE_BIT:		statistic = STAT_TEXT_TYPE_FETCHED_BIT; break;
					case MYSQL_TYPE_NEWDECIMAL:	statistic = STAT_TEXT_TYPE_FETCHED_DECIMAL; break;
					case MYSQL_TYPE_ENUM:		statistic = STAT_TEXT_TYPE_FETCHED_ENUM; break;
					case MYSQL_TYPE_SET:		statistic = STAT_TEXT_TYPE_FETCHED_SET; break;
					case MYSQL_TYPE_JSON:		statistic = STAT_TEXT_TYPE_FETCHED_JSON; break;
					case MYSQL_TYPE_TINY_BLOB:	statistic = STAT_TEXT_TYPE_FETCHED_BLOB; break;
					case MYSQL_TYPE_MEDIUM_BLOB:statistic = STAT_TEXT_TYPE_FETCHED_BLOB; break;
					case MYSQL_TYPE_LONG_BLOB:	statistic = STAT_TEXT_TYPE_FETCHED_BLOB; break;
					case MYSQL_TYPE_BLOB:		statistic = STAT_TEXT_TYPE_FETCHED_BLOB; break;
					case MYSQL_TYPE_VAR_STRING:	statistic = STAT_TEXT_TYPE_FETCHED_STRING; break;
					case MYSQL_TYPE_STRING:		statistic = STAT_TEXT_TYPE_FETCHED_STRING; break;
					case MYSQL_TYPE_GEOMETRY:	statistic = STAT_TEXT_TYPE_FETCHED_GEOMETRY; break;
					default: statistic = STAT_TEXT_TYPE_FETCHED_OTHER; break;
				}
				MYSQLND_INC_CONN_STATISTIC_W_VALUE2(stats, statistic, 1, STAT_BYTES_RECEIVED_PURE_DATA_TEXT, len);
			}
#ifdef MYSQLND_STRING_TO_INT_CONVERSION
			if (as_int_or_float && perm_bind.php_type == IS_LONG) {
				zend_uchar save = *(p + len);
				/* We have to make it ASCIIZ temporarily */
				*(p + len) = '\0';
				if (perm_bind.pack_len < SIZEOF_LONG) {
					/* direct conversion */
					int64_t v =
#ifndef PHP_WIN32
						atoll((char *) p);
#else
						_atoi64((char *) p);
#endif
					ZVAL_LONG(*current_field, (long) v); /* the cast is safe */
				} else {
					uint64_t v =
#ifndef PHP_WIN32
						(uint64_t) atoll((char *) p);
#else
						(uint64_t) _atoi64((char *) p);
#endif
					zend_bool uns = fields_metadata[i].flags & UNSIGNED_FLAG? TRUE:FALSE;
					/* We have to make it ASCIIZ temporarily */
#if SIZEOF_LONG==8
					if (uns == TRUE && v > 9223372036854775807L)
#elif SIZEOF_LONG==4
					if ((uns == TRUE && v > L64(2147483647)) ||
						(uns == FALSE && (( L64(2147483647) < (int64_t) v) ||
						(L64(-2147483648) > (int64_t) v))))
#else
#error Need fix for this architecture
#endif /* SIZEOF */
					{
						ZVAL_STRINGL(*current_field, (char *)p, len, 0);
					} else {
						ZVAL_LONG(*current_field, (long) v); /* the cast is safe */
					}
				}
				*(p + len) = save;
			} else if (as_int_or_float && perm_bind.php_type == IS_DOUBLE) {
				zend_uchar save = *(p + len);
				/* We have to make it ASCIIZ temporarily */
				*(p + len) = '\0';
				ZVAL_DOUBLE(*current_field, atof((char *) p));
				*(p + len) = save;
			} else
#endif /* MYSQLND_STRING_TO_INT_CONVERSION */
			if (fields_metadata[i].type == MYSQL_TYPE_BIT) {
				/*
				  BIT fields are specially handled. As they come as bit mask, we have
				  to convert it to human-readable representation. As the bits take
				  less space in the protocol than the numbers they represent, we don't
				  have enough space in the packet buffer to overwrite inside.
				  Thus, a bit more space is pre-allocated at the end of the buffer,
				  see php_mysqlnd_rowp_read(). And we add the strings at the end.
				  Definitely not nice, _hackish_ :(, but works.
				*/
				zend_uchar *start = bit_area;
				ps_fetch_from_1_to_8_bytes(*current_field, &(fields_metadata[i]), 0, &p, len TSRMLS_CC);
				/*
				  We have advanced in ps_fetch_from_1_to_8_bytes. We should go back because
				  later in this function there will be an advancement.
				*/
				p -= len;
				if (Z_TYPE_PP(current_field) == IS_LONG) {
					bit_area += 1 + sprintf((char *)start, "%ld", Z_LVAL_PP(current_field));
					ZVAL_STRINGL(*current_field, (char *) start, bit_area - start - 1, copy_data);
				} else if (Z_TYPE_PP(current_field) == IS_STRING){
					memcpy(bit_area, Z_STRVAL_PP(current_field), Z_STRLEN_PP(current_field));
					bit_area += Z_STRLEN_PP(current_field);
					*bit_area++ = '\0';
					zval_dtor(*current_field);
					ZVAL_STRINGL(*current_field, (char *) start, bit_area - start - 1, copy_data);
				}
			} else {
				ZVAL_STRINGL(*current_field, (char *)p, len, copy_data);
			}
			p += len;
			last_field_was_string = TRUE;
		}
	}
	if (copy_data == FALSE && last_field_was_string) {
		/* Normal queries: The buffer has one more byte at the end, because we need it */
		row_buffer->ptr[data_size] = '\0';
	}

	DBG_RETURN(PASS);
}
