int URI_FUNC(ComposeQueryEngine)(URI_CHAR * dest,
		const URI_TYPE(QueryList) * queryList,
		int maxChars, int * charsWritten, int * charsRequired,
		UriBool spaceToPlus, UriBool normalizeBreaks) {
	UriBool firstItem = URI_TRUE;
	int ampersandLen = 0;  /* increased to 1 from second item on */
	URI_CHAR * write = dest;

	/* Subtract terminator */
	if (dest == NULL) {
		*charsRequired = 0;
	} else {
		maxChars--;
	}
			
	while (queryList != NULL) {
		const URI_CHAR * const key = queryList->key;
		const URI_CHAR * const value = queryList->value;
		const int worstCase = (normalizeBreaks == URI_TRUE ? 6 : 3);
		const int keyLen = (key == NULL) ? 0 : (int)URI_STRLEN(key);
		const int keyRequiredChars = worstCase * keyLen;
		const int valueLen = (value == NULL) ? 0 : (int)URI_STRLEN(value);
		const int valueRequiredChars = worstCase * valueLen;

		if (dest == NULL) {
			if (firstItem == URI_TRUE) {
				ampersandLen = 1;
				firstItem = URI_FALSE;
			}

			(*charsRequired) += ampersandLen + keyRequiredChars + ((value == NULL)
						? 0
						: 1 + valueRequiredChars);
		} else {
			URI_CHAR * afterKey;

			if ((write - dest) + ampersandLen + keyRequiredChars > maxChars) {
				return URI_ERROR_OUTPUT_TOO_LARGE;
			}
 
 			/* Copy key */
 			if (firstItem == URI_TRUE) {
 				firstItem = URI_FALSE;
 			} else {
 				write[0] = _UT('&');
				write++;
			}
			afterKey = URI_FUNC(EscapeEx)(key, key + keyLen,
					write, spaceToPlus, normalizeBreaks);
			write += (afterKey - write);

			if (value != NULL) {
				URI_CHAR * afterValue;

				if ((write - dest) + 1 + valueRequiredChars > maxChars) {
					return URI_ERROR_OUTPUT_TOO_LARGE;
				}

				/* Copy value */
				write[0] = _UT('=');
				write++;
				afterValue = URI_FUNC(EscapeEx)(value, value + valueLen,
						write, spaceToPlus, normalizeBreaks);
				write += (afterValue - write);
			}
		}

		queryList = queryList->next;
	}

	if (dest != NULL) {
		write[0] = _UT('\0');
		if (charsWritten != NULL) {
			*charsWritten = (int)(write - dest) + 1; /* .. for terminator */
		}
	}

	return URI_SUCCESS;
}
