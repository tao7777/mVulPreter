safe_fprintf(FILE *f, const char *fmt, ...)
{
	char fmtbuff_stack[256]; /* Place to format the printf() string. */
	char outbuff[256]; /* Buffer for outgoing characters. */
	char *fmtbuff_heap; /* If fmtbuff_stack is too small, we use malloc */
	char *fmtbuff;  /* Pointer to fmtbuff_stack or fmtbuff_heap. */
	int fmtbuff_length;
	int length, n;
	va_list ap;
	const char *p;
	unsigned i;
	wchar_t wc;
	char try_wc;

	/* Use a stack-allocated buffer if we can, for speed and safety. */
	fmtbuff_heap = NULL;
	fmtbuff_length = sizeof(fmtbuff_stack);
	fmtbuff = fmtbuff_stack;

	/* Try formatting into the stack buffer. */
	va_start(ap, fmt);
	length = vsnprintf(fmtbuff, fmtbuff_length, fmt, ap);
	va_end(ap);

	/* If the result was too large, allocate a buffer on the heap. */
	while (length < 0 || length >= fmtbuff_length) {
		if (length >= fmtbuff_length)
			fmtbuff_length = length+1;
		else if (fmtbuff_length < 8192)
			fmtbuff_length *= 2;
		else if (fmtbuff_length < 1000000)
			fmtbuff_length += fmtbuff_length / 4;
		else {
			length = fmtbuff_length;
			fmtbuff_heap[length-1] = '\0';
			break;
		}
		free(fmtbuff_heap);
		fmtbuff_heap = malloc(fmtbuff_length);

		/* Reformat the result into the heap buffer if we can. */
		if (fmtbuff_heap != NULL) {
			fmtbuff = fmtbuff_heap;
			va_start(ap, fmt);
			length = vsnprintf(fmtbuff, fmtbuff_length, fmt, ap);
			va_end(ap);
		} else {
			/* Leave fmtbuff pointing to the truncated
			 * string in fmtbuff_stack. */
			length = sizeof(fmtbuff_stack) - 1;
			break;
		}
	}

	/* Note: mbrtowc() has a cleaner API, but mbtowc() seems a bit
	 * more portable, so we use that here instead. */
	if (mbtowc(NULL, NULL, 1) == -1) { /* Reset the shift state. */
		/* mbtowc() should never fail in practice, but
		 * handle the theoretical error anyway. */
		free(fmtbuff_heap);
		return;
	}

	/* Write data, expanding unprintable characters. */
	p = fmtbuff;
	i = 0;
	try_wc = 1;
	while (*p != '\0') {

		/* Convert to wide char, test if the wide
		 * char is printable in the current locale. */
		if (try_wc && (n = mbtowc(&wc, p, length)) != -1) {
			length -= n;
			if (iswprint(wc) && wc != L'\\') {
				/* Printable, copy the bytes through. */
				while (n-- > 0)
					outbuff[i++] = *p++;
			} else {
				/* Not printable, format the bytes. */
				while (n-- > 0)
					i += (unsigned)bsdtar_expand_char(
					    outbuff, i, *p++);
			}
		} else {
			/* After any conversion failure, don't bother
			 * trying to convert the rest. */
			i += (unsigned)bsdtar_expand_char(outbuff, i, *p++);
			try_wc = 0;
 		}
 
 		/* If our output buffer is full, dump it and keep going. */
		if (i > (sizeof(outbuff) - 128)) {
 			outbuff[i] = '\0';
 			fprintf(f, "%s", outbuff);
 			i = 0;
		}
	}
	outbuff[i] = '\0';
	fprintf(f, "%s", outbuff);

	/* If we allocated a heap-based formatting buffer, free it now. */
	free(fmtbuff_heap);
}
