static int string_scan_range(RList *list, RBinFile *bf, int min,
			      const ut64 from, const ut64 to, int type) {
	ut8 tmp[R_STRING_SCAN_BUFFER_SIZE];
	ut64 str_start, needle = from;
	int count = 0, i, rc, runes;
	int str_type = R_STRING_TYPE_DETECT;

	if (type == -1) {
		type = R_STRING_TYPE_DETECT;
	}
	if (from >= to) {
 		eprintf ("Invalid range to find strings 0x%llx .. 0x%llx\n", from, to);
 		return -1;
 	}
	int len = to - from;
	ut8 *buf = calloc (len, 1);
 	if (!buf || !min) {
 		return -1;
 	}
	r_buf_read_at (bf->buf, from, buf, len);
 	while (needle < to) {
 		rc = r_utf8_decode (buf + needle - from, to - needle, NULL);
		if (!rc) {
			needle++;
			continue;
 		}
 		if (type == R_STRING_TYPE_DETECT) {
 			char *w = (char *)buf + needle + rc - from;
			if ((to - needle) > 5 + rc) {
				bool is_wide32 = (needle + rc + 2 < to) && (!w[0] && !w[1] && !w[2] && w[3] && !w[4]);
 				if (is_wide32) {
 					str_type = R_STRING_TYPE_WIDE32;
 				} else {
					bool is_wide = needle + rc + 2 < to && !w[0] && w[1] && !w[2];
					str_type = is_wide? R_STRING_TYPE_WIDE: R_STRING_TYPE_ASCII;
				}
			} else {
				str_type = R_STRING_TYPE_ASCII;
			}
		} else {
			str_type = type;
		}
		runes = 0;
		str_start = needle;

		/* Eat a whole C string */
		for (rc = i = 0; i < sizeof (tmp) - 3 && needle < to; i += rc) {
			RRune r = {0};

			if (str_type == R_STRING_TYPE_WIDE32) {
				rc = r_utf32le_decode (buf + needle - from, to - needle, &r);
				if (rc) {
					rc = 4;
				}
			} else if (str_type == R_STRING_TYPE_WIDE) {
				rc = r_utf16le_decode (buf + needle - from, to - needle, &r);
				if (rc == 1) {
					rc = 2;
				}
			} else {
				rc = r_utf8_decode (buf + needle - from, to - needle, &r);
				if (rc > 1) {
					str_type = R_STRING_TYPE_UTF8;
				}
			}

			/* Invalid sequence detected */
			if (!rc) {
				needle++;
				break;
			}

			needle += rc;

			if (r_isprint (r) && r != '\\') {
				if (str_type == R_STRING_TYPE_WIDE32) {
					if (r == 0xff) {
						r = 0;
					}
				}
				rc = r_utf8_encode (&tmp[i], r);
				runes++;
				/* Print the escape code */
			} else if (r && r < 0x100 && strchr ("\b\v\f\n\r\t\a\033\\", (char)r)) {
				if ((i + 32) < sizeof (tmp) && r < 93) {
					tmp[i + 0] = '\\';
					tmp[i + 1] = "       abtnvfr             e  "
					             "                              "
					             "                              "
					             "  \\"[r];
				} else {
					break;
				}
				rc = 2;
				runes++;
			} else {
				/* \0 marks the end of C-strings */
				break;
			}
		}

		tmp[i++] = '\0';

		if (runes >= min) {
			if (str_type == R_STRING_TYPE_ASCII) {
				int j;
				for (j = 0; j < i; j++) {
					char ch = tmp[j];
					if (ch != '\n' && ch != '\r' && ch != '\t') {
						if (!IS_PRINTABLE (tmp[j])) {
							continue;
						}
					}
				}
			}
			RBinString *bs = R_NEW0 (RBinString);
			if (!bs) {
				break;
			}
			bs->type = str_type;
			bs->length = runes;
			bs->size = needle - str_start;
			bs->ordinal = count++;
			switch (str_type) {
			case R_STRING_TYPE_WIDE:
				if (str_start -from> 1) {
					const ut8 *p = buf + str_start - 2 - from;
					if (p[0] == 0xff && p[1] == 0xfe) {
						str_start -= 2; // \xff\xfe
					}
				}
				break;
			case R_STRING_TYPE_WIDE32:
				if (str_start -from> 3) {
					const ut8 *p = buf + str_start - 4 - from;
					if (p[0] == 0xff && p[1] == 0xfe) {
						str_start -= 4; // \xff\xfe\x00\x00
					}
				}
				break;
			}
			bs->paddr = bs->vaddr = str_start;
			bs->string = r_str_ndup ((const char *)tmp, i);
			if (list) {
				r_list_append (list, bs);
			} else {
				print_string (bs, bf);
				r_bin_string_free (bs);
			}
		}
	}
	free (buf);
	return count;
}
