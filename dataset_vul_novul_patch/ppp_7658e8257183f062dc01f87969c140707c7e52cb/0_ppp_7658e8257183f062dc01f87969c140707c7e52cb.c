getword(f, word, newlinep, filename)
    FILE *f;
    char *word;
    int *newlinep;
    char *filename;
{
    int c, len, escape;
    int quoted, comment;
    int value, digit, got, n;

#define isoctal(c) ((c) >= '0' && (c) < '8')

    *newlinep = 0;
    len = 0;
    escape = 0;
    comment = 0;
    quoted = 0;

    /*
     * First skip white-space and comments.
     */
    for (;;) {
	c = getc(f);
	if (c == EOF)
	    break;

	/*
	 * A newline means the end of a comment; backslash-newline
	 * is ignored.  Note that we cannot have escape && comment.
	 */
	if (c == '\n') {
	    if (!escape) {
		*newlinep = 1;
		comment = 0;
	    } else
		escape = 0;
	    continue;
	}

	/*
	 * Ignore characters other than newline in a comment.
	 */
	if (comment)
	    continue;

	/*
	 * If this character is escaped, we have a word start.
	 */
	if (escape)
	    break;

	/*
	 * If this is the escape character, look at the next character.
	 */
	if (c == '\\') {
	    escape = 1;
	    continue;
	}

	/*
	 * If this is the start of a comment, ignore the rest of the line.
	 */
	if (c == '#') {
	    comment = 1;
	    continue;
	}

	/*
	 * A non-whitespace character is the start of a word.
	 */
	if (!isspace(c))
	    break;
    }

    /*
     * Process characters until the end of the word.
     */
    while (c != EOF) {
	if (escape) {
	    /*
	     * This character is escaped: backslash-newline is ignored,
	     * various other characters indicate particular values
	     * as for C backslash-escapes.
	     */
	    escape = 0;
	    if (c == '\n') {
	        c = getc(f);
		continue;
	    }

	    got = 0;
	    switch (c) {
	    case 'a':
		value = '\a';
		break;
	    case 'b':
		value = '\b';
		break;
	    case 'f':
		value = '\f';
		break;
	    case 'n':
		value = '\n';
		break;
	    case 'r':
		value = '\r';
		break;
	    case 's':
		value = ' ';
		break;
	    case 't':
		value = '\t';
		break;

	    default:
		if (isoctal(c)) {
		    /*
		     * \ddd octal sequence
		     */
		    value = 0;
		    for (n = 0; n < 3 && isoctal(c); ++n) {
			value = (value << 3) + (c & 07);
			c = getc(f);
		    }
		    got = 1;
		    break;
		}

		if (c == 'x') {
		    /*
		     * \x<hex_string> sequence
		     */
		    value = 0;
		    c = getc(f);
		    for (n = 0; n < 2 && isxdigit(c); ++n) {
			digit = toupper(c) - '0';
			if (digit > 10)
			    digit += '0' + 10 - 'A';
			value = (value << 4) + digit;
			c = getc (f);
		    }
		    got = 1;
		    break;
		}

		/*
		 * Otherwise the character stands for itself.
		 */
		value = c;
		break;
	    }

 	    /*
 	     * Store the resulting character for the escape sequence.
 	     */
	    if (len < MAXWORDLEN) {
 		word[len] = value;
		++len;
	    }
 
 	    if (!got)
 		c = getc(f);
	    continue;
	}

	/*
	 * Backslash starts a new escape sequence.
	 */
	if (c == '\\') {
	    escape = 1;
	    c = getc(f);
	    continue;
	}

	/*
	 * Not escaped: check for the start or end of a quoted
	 * section and see if we've reached the end of the word.
	 */
	if (quoted) {
	    if (c == quoted) {
		quoted = 0;
		c = getc(f);
		continue;
	    }
	} else if (c == '"' || c == '\'') {
	    quoted = c;
	    c = getc(f);
	    continue;
	} else if (isspace(c) || c == '#') {
	    ungetc (c, f);
	    break;
	}

 	/*
 	 * An ordinary character: store it in the word and get another.
 	 */
	if (len < MAXWORDLEN) {
 	    word[len] = c;
	    ++len;
	}
 
 	c = getc(f);
     }

    /*
     * End of the word: check for errors.
     */
    if (c == EOF) {
	if (ferror(f)) {
	    if (errno == 0)
		errno = EIO;
	    option_error("Error reading %s: %m", filename);
	    die(1);
	}
	/*
	 * If len is zero, then we didn't find a word before the
	 * end of the file.
	 */
	if (len == 0)
	    return 0;
	if (quoted)
	    option_error("warning: quoted word runs to end of file (%.20s...)",
			 filename, word);
    }

    /*
     * Warn if the word was too long, and append a terminating null.
     */
    if (len >= MAXWORDLEN) {
	option_error("warning: word in file %s too long (%.20s...)",
		     filename, word);
	len = MAXWORDLEN - 1;
    }
    word[len] = 0;

    return 1;

#undef isoctal

}
