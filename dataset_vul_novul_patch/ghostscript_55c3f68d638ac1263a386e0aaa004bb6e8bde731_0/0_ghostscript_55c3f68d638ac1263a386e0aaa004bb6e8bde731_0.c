pdf_read_old_xref(fz_context *ctx, pdf_document *doc, pdf_lexbuf *buf)
{
	fz_stream *file = doc->file;

	int64_t ofs;
	int len;
	char *s;
	size_t n;
	pdf_token tok;
	int64_t i;
	int c;
	int xref_len = pdf_xref_size_from_old_trailer(ctx, doc, buf);
	pdf_xref_entry *table;
	int carried;

	fz_skip_space(ctx, doc->file);
	if (fz_skip_string(ctx, doc->file, "xref"))
		fz_throw(ctx, FZ_ERROR_GENERIC, "cannot find xref marker");
	fz_skip_space(ctx, doc->file);

	while (1)
	{
		c = fz_peek_byte(ctx, file);
		if (!(c >= '0' && c <= '9'))
			break;

		fz_read_line(ctx, file, buf->scratch, buf->size);
		s = buf->scratch;
		ofs = fz_atoi64(fz_strsep(&s, " "));
		len = fz_atoi(fz_strsep(&s, " "));

		/* broken pdfs where the section is not on a separate line */
		if (s && *s != '\0')
		{
			fz_warn(ctx, "broken xref section. proceeding anyway.");
                        fz_seek(ctx, file, -(2 + (int)strlen(s)), SEEK_CUR);
                }
 
               if (ofs < 0 || ofs > PDF_MAX_OBJECT_NUMBER
                               || len < 0 || len > PDF_MAX_OBJECT_NUMBER
                               || ofs + len - 1 > PDF_MAX_OBJECT_NUMBER)
               {
                       fz_throw(ctx, FZ_ERROR_GENERIC, "xref subsection object numbers are out of range");
               }
                /* broken pdfs where size in trailer undershoots entries in xref sections */
                if (ofs + len > xref_len)
                {
		}

		table = pdf_xref_find_subsection(ctx, doc, ofs, len);

		/* Xref entries SHOULD be 20 bytes long, but we see 19 byte
		 * ones more frequently than we'd like (e.g. PCLm drivers).
		 * Cope with this by 'carrying' data forward. */
		carried = 0;
		for (i = ofs; i < ofs + len; i++)
		{
			pdf_xref_entry *entry = &table[i-ofs];
			n = fz_read(ctx, file, (unsigned char *) buf->scratch + carried, 20-carried);
			if (n != 20-carried)
				fz_throw(ctx, FZ_ERROR_GENERIC, "unexpected EOF in xref table");
			n += carried;
			if (!entry->type)
			{
				s = buf->scratch;

				/* broken pdfs where line start with white space */
				while (*s != '\0' && iswhite(*s))
					s++;

				entry->ofs = fz_atoi64(s);
				entry->gen = fz_atoi(s + 11);
				entry->num = (int)i;
				entry->type = s[17];
				if (s[17] != 'f' && s[17] != 'n' && s[17] != 'o')
					fz_throw(ctx, FZ_ERROR_GENERIC, "unexpected xref type: 0x%x (%d %d R)", s[17], entry->num, entry->gen);
				/* If the last byte of our buffer isn't an EOL (or space), carry one byte forward */
				carried = s[19] > 32;
				if (carried)
					s[0] = s[19];
			}
		}
		if (carried)
			fz_unread_byte(ctx, file);
	}

	tok = pdf_lex(ctx, file, buf);
	if (tok != PDF_TOK_TRAILER)
		fz_throw(ctx, FZ_ERROR_GENERIC, "expected trailer marker");

	tok = pdf_lex(ctx, file, buf);
	if (tok != PDF_TOK_OPEN_DICT)
		fz_throw(ctx, FZ_ERROR_GENERIC, "expected trailer dictionary");

	return pdf_parse_dict(ctx, doc, file, buf);
}
