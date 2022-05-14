another_hunk (enum diff difftype, bool rev)
{
    char *s;
    lin context = 0;
    size_t chars_read;
    char numbuf0[LINENUM_LENGTH_BOUND + 1];
     char numbuf2[LINENUM_LENGTH_BOUND + 1];
     char numbuf3[LINENUM_LENGTH_BOUND + 1];
 
    set_hunkmax();

     while (p_end >= 0) {
 	if (p_end == p_efake)
 	    p_end = p_bfake;		/* don't free twice */
	    free(p_line[p_end]);
	p_end--;
    }
    assert(p_end == -1);
    p_efake = -1;

    if (p_c_function)
      {
	free (p_c_function);
	p_c_function = NULL;
      }

    p_max = hunkmax;			/* gets reduced when --- found */
    if (difftype == CONTEXT_DIFF || difftype == NEW_CONTEXT_DIFF) {
	file_offset line_beginning = file_tell (pfp);
					/* file pos of the current line */
	lin repl_beginning = 0;		/* index of --- line */
	lin fillcnt = 0;	/* #lines of missing ptrn or repl */
	lin fillsrc;		/* index of first line to copy */
	lin filldst;		/* index of first missing line */
	bool ptrn_spaces_eaten = false;	/* ptrn was slightly misformed */
	bool some_context = false;	/* (perhaps internal) context seen */
	bool repl_could_be_missing = true;
	bool ptrn_missing = false;	/* The pattern was missing.  */
	bool repl_missing = false;	/* Likewise for replacement.  */
	file_offset repl_backtrack_position = 0;
					/* file pos of first repl line */
	lin repl_patch_line;		/* input line number for same */
	lin repl_context;		/* context for same */
	lin ptrn_prefix_context = -1;	/* lines in pattern prefix context */
	lin ptrn_suffix_context = -1;	/* lines in pattern suffix context */
	lin repl_prefix_context = -1;	/* lines in replac. prefix context */
	lin ptrn_copiable = 0;		/* # of copiable lines in ptrn */
	lin repl_copiable = 0;		/* Likewise for replacement.  */

	/* Pacify 'gcc -Wall'.  */
	fillsrc = filldst = repl_patch_line = repl_context = 0;

	chars_read = get_line ();
	if (chars_read == (size_t) -1
	    || chars_read <= 8
	    || strncmp (buf, "********", 8) != 0) {
	    next_intuit_at(line_beginning,p_input_line);
	    return chars_read == (size_t) -1 ? -1 : 0;
	}
	s = buf;
	while (*s == '*')
	    s++;
	if (*s == ' ')
	  {
	    p_c_function = s;
	    while (*s != '\n')
		s++;
	    *s = '\0';
	    p_c_function = savestr (p_c_function);
	    if (! p_c_function)
	      return -1;
	  }
	p_hunk_beg = p_input_line + 1;
	while (p_end < p_max) {
	    chars_read = get_line ();
	    if (chars_read == (size_t) -1)
	      return -1;
	    if (!chars_read) {
		if (repl_beginning && repl_could_be_missing) {
		    repl_missing = true;
		    goto hunk_done;
		}
		if (p_max - p_end < 4) {
		    strcpy (buf, "  \n");  /* assume blank lines got chopped */
		    chars_read = 3;
		} else {
		    fatal ("unexpected end of file in patch");
		}
	    }
	    p_end++;
	    if (p_end == hunkmax)
	      fatal ("unterminated hunk starting at line %s; giving up at line %s: %s",
		     format_linenum (numbuf0, pch_hunk_beg ()),
		     format_linenum (numbuf1, p_input_line), buf);
	    assert(p_end < hunkmax);
	    p_Char[p_end] = *buf;
	    p_len[p_end] = 0;
	    p_line[p_end] = 0;
	    switch (*buf) {
	    case '*':
		if (strnEQ(buf, "********", 8)) {
		    if (repl_beginning && repl_could_be_missing) {
			repl_missing = true;
			goto hunk_done;
		    }
		    else
		      fatal ("unexpected end of hunk at line %s",
			     format_linenum (numbuf0, p_input_line));
		}
		if (p_end != 0) {
		    if (repl_beginning && repl_could_be_missing) {
			repl_missing = true;
			goto hunk_done;
		    }
		    fatal ("unexpected '***' at line %s: %s",
			   format_linenum (numbuf0, p_input_line), buf);
		}
		context = 0;
		p_len[p_end] = strlen (buf);
		if (! (p_line[p_end] = savestr (buf))) {
		    p_end--;
		    return -1;
		}
		for (s = buf;  *s && !ISDIGIT (*s);  s++)
		  /* do nothing */ ;
		if (strnEQ(s,"0,0",3))
		    remove_prefix (s, 2);
		s = scan_linenum (s, &p_first);
		if (*s == ',') {
		    while (*s && !ISDIGIT (*s))
		      s++;
		    scan_linenum (s, &p_ptrn_lines);
		    p_ptrn_lines += 1 - p_first;
		    if (p_ptrn_lines < 0)
		      malformed ();
		}
		else if (p_first)
		    p_ptrn_lines = 1;
		else {
		    p_ptrn_lines = 0;
		    p_first = 1;
		}
		if (p_first >= LINENUM_MAX - p_ptrn_lines ||
		    p_ptrn_lines >= LINENUM_MAX - 6)
		  malformed ();
		p_max = p_ptrn_lines + 6;	/* we need this much at least */
		while (p_max + 1 >= hunkmax)
		    if (! grow_hunkmax ())
			return -1;
		p_max = hunkmax;
		break;
	    case '-':
		if (buf[1] != '-')
		  goto change_line;
		if (ptrn_prefix_context == -1)
		  ptrn_prefix_context = context;
		ptrn_suffix_context = context;
		if (repl_beginning
		    || (p_end
			!= p_ptrn_lines + 1 + (p_Char[p_end - 1] == '\n')))
		  {
		    if (p_end == 1)
		      {
			/* 'Old' lines were omitted.  Set up to fill
			   them in from 'new' context lines.  */
			ptrn_missing = true;
			p_end = p_ptrn_lines + 1;
			ptrn_prefix_context = ptrn_suffix_context = -1;
			fillsrc = p_end + 1;
			filldst = 1;
			fillcnt = p_ptrn_lines;
		      }
		    else if (! repl_beginning)
		      fatal ("%s '---' at line %s; check line numbers at line %s",
			     (p_end <= p_ptrn_lines
			      ? "Premature"
			      : "Overdue"),
			     format_linenum (numbuf0, p_input_line),
			     format_linenum (numbuf1, p_hunk_beg));
		    else if (! repl_could_be_missing)
		      fatal ("duplicate '---' at line %s; check line numbers at line %s",
			     format_linenum (numbuf0, p_input_line),
			     format_linenum (numbuf1,
					     p_hunk_beg + repl_beginning));
		    else
		      {
			repl_missing = true;
			goto hunk_done;
		      }
		  }
		repl_beginning = p_end;
		repl_backtrack_position = file_tell (pfp);
		repl_patch_line = p_input_line;
		repl_context = context;
		p_len[p_end] = strlen (buf);
		if (! (p_line[p_end] = savestr (buf)))
		  {
		    p_end--;
		    return -1;
		  }
		p_Char[p_end] = '=';
		for (s = buf;  *s && ! ISDIGIT (*s);  s++)
		  /* do nothing */ ;
		s = scan_linenum (s, &p_newfirst);
		if (*s == ',')
		  {
		    do
		      {
			if (!*++s)
			  malformed ();
		      }
		    while (! ISDIGIT (*s));
		    scan_linenum (s, &p_repl_lines);
		    p_repl_lines += 1 - p_newfirst;
		    if (p_repl_lines < 0)
		      malformed ();
		  }
		else if (p_newfirst)
		  p_repl_lines = 1;
		else
		  {
		    p_repl_lines = 0;
		    p_newfirst = 1;
		  }
		if (p_newfirst >= LINENUM_MAX - p_repl_lines ||
		    p_repl_lines >= LINENUM_MAX - p_end)
		  malformed ();
		p_max = p_repl_lines + p_end;
		while (p_max + 1 >= hunkmax)
		  if (! grow_hunkmax ())
		    return -1;
		if (p_repl_lines != ptrn_copiable
		    && (p_prefix_context != 0
			|| context != 0
			|| p_repl_lines != 1))
		  repl_could_be_missing = false;
		context = 0;
		break;
	    case '+':  case '!':
		repl_could_be_missing = false;
	      change_line:
		s = buf + 1;
		chars_read--;
		if (*s == '\n' && canonicalize) {
		    strcpy (s, " \n");
		    chars_read = 2;
		}
		if (*s == ' ' || *s == '\t') {
		    s++;
		    chars_read--;
		} else if (repl_beginning && repl_could_be_missing) {
		    repl_missing = true;
		    goto hunk_done;
		}
		if (! repl_beginning)
		  {
		    if (ptrn_prefix_context == -1)
		      ptrn_prefix_context = context;
		  }
		else
		  {
		    if (repl_prefix_context == -1)
		      repl_prefix_context = context;
		  }
		chars_read -=
		  (1 < chars_read
		   && p_end == (repl_beginning ? p_max : p_ptrn_lines)
		   && incomplete_line ());
		p_len[p_end] = chars_read;
		p_line[p_end] = savebuf (s, chars_read);
		if (chars_read && ! p_line[p_end]) {
		    p_end--;
		    return -1;
	        }
		context = 0;
		break;
	    case '\t': case '\n':	/* assume spaces got eaten */
		s = buf;
		if (*buf == '\t') {
		    s++;
		    chars_read--;
		}
		if (repl_beginning && repl_could_be_missing &&
		    (!ptrn_spaces_eaten || difftype == NEW_CONTEXT_DIFF) ) {
		    repl_missing = true;
		    goto hunk_done;
		}
		chars_read -=
		  (1 < chars_read
		   && p_end == (repl_beginning ? p_max : p_ptrn_lines)
		   && incomplete_line ());
		p_len[p_end] = chars_read;
		p_line[p_end] = savebuf (buf, chars_read);
		if (chars_read && ! p_line[p_end]) {
		    p_end--;
		    return -1;
		}
		if (p_end != p_ptrn_lines + 1) {
		    ptrn_spaces_eaten |= (repl_beginning != 0);
		    some_context = true;
		    context++;
		    if (repl_beginning)
			repl_copiable++;
		    else
			ptrn_copiable++;
		    p_Char[p_end] = ' ';
		}
		break;
	    case ' ':
		s = buf + 1;
		chars_read--;
		if (*s == '\n' && canonicalize) {
		    strcpy (s, "\n");
		    chars_read = 2;
		}
		if (*s == ' ' || *s == '\t') {
		    s++;
		    chars_read--;
		} else if (repl_beginning && repl_could_be_missing) {
		    repl_missing = true;
		    goto hunk_done;
		}
		some_context = true;
		context++;
		if (repl_beginning)
		    repl_copiable++;
		else
		    ptrn_copiable++;
		chars_read -=
		  (1 < chars_read
		   && p_end == (repl_beginning ? p_max : p_ptrn_lines)
		   && incomplete_line ());
		p_len[p_end] = chars_read;
		p_line[p_end] = savebuf (s, chars_read);
		if (chars_read && ! p_line[p_end]) {
		    p_end--;
		    return -1;
		}
		break;
	    default:
		if (repl_beginning && repl_could_be_missing) {
		    repl_missing = true;
		    goto hunk_done;
		}
		malformed ();
	    }
	}

    hunk_done:
	if (p_end >=0 && !repl_beginning)
	  fatal ("no '---' found in patch at line %s",
		 format_linenum (numbuf0, pch_hunk_beg ()));

	if (repl_missing) {

	    /* reset state back to just after --- */
	    p_input_line = repl_patch_line;
	    context = repl_context;
	    for (p_end--; p_end > repl_beginning; p_end--)
		free(p_line[p_end]);
	    Fseek (pfp, repl_backtrack_position, SEEK_SET);

	    /* redundant 'new' context lines were omitted - set */
	    /* up to fill them in from the old file context */
	    fillsrc = 1;
	    filldst = repl_beginning+1;
	    fillcnt = p_repl_lines;
	    p_end = p_max;
	}
	else if (! ptrn_missing && ptrn_copiable != repl_copiable)
	  fatal ("context mangled in hunk at line %s",
		 format_linenum (numbuf0, p_hunk_beg));
	else if (!some_context && fillcnt == 1) {
	    /* the first hunk was a null hunk with no context */
	    /* and we were expecting one line -- fix it up. */
	    while (filldst < p_end) {
		p_line[filldst] = p_line[filldst+1];
		p_Char[filldst] = p_Char[filldst+1];
		p_len[filldst] = p_len[filldst+1];
		filldst++;
	    }
#if 0
	    repl_beginning--;		/* this doesn't need to be fixed */
#endif
	    p_end--;
	    p_first++;			/* do append rather than insert */
	    fillcnt = 0;
	    p_ptrn_lines = 0;
	}

	p_prefix_context = ((repl_prefix_context == -1
			     || (ptrn_prefix_context != -1
				 && ptrn_prefix_context < repl_prefix_context))
			    ? ptrn_prefix_context : repl_prefix_context);
	p_suffix_context = ((ptrn_suffix_context != -1
			     && ptrn_suffix_context < context)
			    ? ptrn_suffix_context : context);
	if (p_prefix_context == -1 || p_suffix_context == -1)
	    fatal ("replacement text or line numbers mangled in hunk at line %s",
		   format_linenum (numbuf0, p_hunk_beg));

	if (difftype == CONTEXT_DIFF
	    && (fillcnt
		|| (p_first > 1
		    && p_prefix_context + p_suffix_context < ptrn_copiable))) {
	    if (verbosity == VERBOSE)
		say ("%s\n%s\n%s\n",
"(Fascinating -- this is really a new-style context diff but without",
"the telltale extra asterisks on the *** line that usually indicate",
"the new style...)");
	    diff_type = difftype = NEW_CONTEXT_DIFF;
	}

	/* if there were omitted context lines, fill them in now */
	if (fillcnt) {
	    p_bfake = filldst;		/* remember where not to free() */
	    p_efake = filldst + fillcnt - 1;
	    while (fillcnt-- > 0) {
		while (fillsrc <= p_end && fillsrc != repl_beginning
		       && p_Char[fillsrc] != ' ')
		    fillsrc++;
		if (p_end < fillsrc || fillsrc == repl_beginning)
		  {
		    fatal ("replacement text or line numbers mangled in hunk at line %s",
			   format_linenum (numbuf0, p_hunk_beg));
		  }
		p_line[filldst] = p_line[fillsrc];
		p_Char[filldst] = p_Char[fillsrc];
		p_len[filldst] = p_len[fillsrc];
		fillsrc++; filldst++;
	    }
	    while (fillsrc <= p_end && fillsrc != repl_beginning)
	      {
		if (p_Char[fillsrc] == ' ')
		  fatal ("replacement text or line numbers mangled in hunk at line %s",
			 format_linenum (numbuf0, p_hunk_beg));
		fillsrc++;
	      }
	    if (debug & 64)
	      printf ("fillsrc %s, filldst %s, rb %s, e+1 %s\n",
		      format_linenum (numbuf0, fillsrc),
		      format_linenum (numbuf1, filldst),
		      format_linenum (numbuf2, repl_beginning),
		      format_linenum (numbuf3, p_end + 1));
	    assert(fillsrc==p_end+1 || fillsrc==repl_beginning);
	    assert(filldst==p_end+1 || filldst==repl_beginning);
	}
    }
    else if (difftype == UNI_DIFF) {
	file_offset line_beginning = file_tell (pfp);  /* file pos of the current line */
	lin fillsrc;  /* index of old lines */
	lin filldst;  /* index of new lines */
	char ch = '\0';

	chars_read = get_line ();
	if (chars_read == (size_t) -1
	    || chars_read <= 4
	    || strncmp (buf, "@@ -", 4) != 0) {
	    next_intuit_at(line_beginning,p_input_line);
	    return chars_read == (size_t) -1 ? -1 : 0;
	}
	s = scan_linenum (buf + 4, &p_first);
	if (*s == ',')
	    s = scan_linenum (s + 1, &p_ptrn_lines);
	else
	    p_ptrn_lines = 1;
	if (p_first >= LINENUM_MAX - p_ptrn_lines)
	  malformed ();
	if (*s == ' ') s++;
	if (*s != '+')
	    malformed ();
	s = scan_linenum (s + 1, &p_newfirst);
	if (*s == ',')
	    s = scan_linenum (s + 1, &p_repl_lines);
	else
	    p_repl_lines = 1;
	if (p_newfirst >= LINENUM_MAX - p_repl_lines)
	  malformed ();
	if (*s == ' ') s++;
	if (*s++ != '@')
	    malformed ();
	if (*s++ == '@' && *s == ' ')
	  {
	    p_c_function = s;
	    while (*s != '\n')
		s++;
	    *s = '\0';
	    p_c_function = savestr (p_c_function);
	    if (! p_c_function)
	      return -1;
	  }
	if (!p_ptrn_lines)
	    p_first++;			/* do append rather than insert */
	if (!p_repl_lines)
	    p_newfirst++;
	if (p_ptrn_lines >= LINENUM_MAX - (p_repl_lines + 1))
	  malformed ();
	p_max = p_ptrn_lines + p_repl_lines + 1;
	while (p_max + 1 >= hunkmax)
	    if (! grow_hunkmax ())
		return -1;
	fillsrc = 1;
	filldst = fillsrc + p_ptrn_lines;
	p_end = filldst + p_repl_lines;
	sprintf (buf, "*** %s,%s ****\n",
		 format_linenum (numbuf0, p_first),
		 format_linenum (numbuf1, p_first + p_ptrn_lines - 1));
	p_len[0] = strlen (buf);
	if (! (p_line[0] = savestr (buf))) {
	    p_end = -1;
	    return -1;
	}
	p_Char[0] = '*';
	sprintf (buf, "--- %s,%s ----\n",
		 format_linenum (numbuf0, p_newfirst),
		 format_linenum (numbuf1, p_newfirst + p_repl_lines - 1));
	p_len[filldst] = strlen (buf);
	if (! (p_line[filldst] = savestr (buf))) {
	    p_end = 0;
	    return -1;
	}
	p_Char[filldst++] = '=';
	p_prefix_context = -1;
	p_hunk_beg = p_input_line + 1;
	while (fillsrc <= p_ptrn_lines || filldst <= p_end) {
	    chars_read = get_line ();
	    if (!chars_read) {
		if (p_max - filldst < 3) {
		    strcpy (buf, " \n");  /* assume blank lines got chopped */
		    chars_read = 2;
		} else {
		    fatal ("unexpected end of file in patch");
		}
	    }
	    if (chars_read == (size_t) -1)
		s = 0;
	    else if (*buf == '\t' || *buf == '\n') {
		ch = ' ';		/* assume the space got eaten */
		s = savebuf (buf, chars_read);
	    }
	    else {
		ch = *buf;
		s = savebuf (buf+1, --chars_read);
	    }
	    if (chars_read && ! s)
	      {
		while (--filldst > p_ptrn_lines)
		    free(p_line[filldst]);
		p_end = fillsrc-1;
		return -1;
	      }
	    switch (ch) {
	    case '-':
		if (fillsrc > p_ptrn_lines) {
		    free(s);
		    p_end = filldst-1;
		    malformed ();
		}
		chars_read -= fillsrc == p_ptrn_lines && incomplete_line ();
		p_Char[fillsrc] = ch;
		p_line[fillsrc] = s;
		p_len[fillsrc++] = chars_read;
		break;
	    case '=':
		ch = ' ';
		/* FALL THROUGH */
	    case ' ':
		if (fillsrc > p_ptrn_lines) {
		    free(s);
		    while (--filldst > p_ptrn_lines)
			free(p_line[filldst]);
		    p_end = fillsrc-1;
		    malformed ();
		}
		context++;
		chars_read -= fillsrc == p_ptrn_lines && incomplete_line ();
		p_Char[fillsrc] = ch;
		p_line[fillsrc] = s;
		p_len[fillsrc++] = chars_read;
		s = savebuf (s, chars_read);
		if (chars_read && ! s) {
		    while (--filldst > p_ptrn_lines)
			free(p_line[filldst]);
		    p_end = fillsrc-1;
		    return -1;
		}
		/* FALL THROUGH */
	    case '+':
		if (filldst > p_end) {
		    free(s);
		    while (--filldst > p_ptrn_lines)
			free(p_line[filldst]);
		    p_end = fillsrc-1;
		    malformed ();
		}
		chars_read -= filldst == p_end && incomplete_line ();
		p_Char[filldst] = ch;
		p_line[filldst] = s;
		p_len[filldst++] = chars_read;
		break;
	    default:
		p_end = filldst;
		malformed ();
	    }
	    if (ch != ' ') {
		if (p_prefix_context == -1)
		    p_prefix_context = context;
		context = 0;
	    }
	}/* while */
	if (p_prefix_context == -1)
	  malformed ();
	p_suffix_context = context;
    }
    else {				/* normal diff--fake it up */
	char hunk_type;
	int i;
	lin min, max;
	file_offset line_beginning = file_tell (pfp);

	p_prefix_context = p_suffix_context = 0;
	chars_read = get_line ();
	if (chars_read == (size_t) -1 || !chars_read || !ISDIGIT (*buf)) {
	    next_intuit_at(line_beginning,p_input_line);
	    return chars_read == (size_t) -1 ? -1 : 0;
	}
	s = scan_linenum (buf, &p_first);
	if (*s == ',') {
	    s = scan_linenum (s + 1, &p_ptrn_lines);
	    p_ptrn_lines += 1 - p_first;
	}
	else
	    p_ptrn_lines = (*s != 'a');
	if (p_first >= LINENUM_MAX - p_ptrn_lines)
	  malformed ();
	hunk_type = *s;
	if (hunk_type == 'a')
	    p_first++;			/* do append rather than insert */
	s = scan_linenum (s + 1, &min);
	if (*s == ',')
	    scan_linenum (s + 1, &max);
	else
	    max = min;
	if (min > max || max - min == LINENUM_MAX)
	  malformed ();
	if (hunk_type == 'd')
	    min++;
	p_newfirst = min;
	p_repl_lines = max - min + 1;
	if (p_newfirst >= LINENUM_MAX - p_repl_lines)
	  malformed ();
	if (p_ptrn_lines >= LINENUM_MAX - (p_repl_lines + 1))
	  malformed ();
	p_end = p_ptrn_lines + p_repl_lines + 1;
	while (p_end + 1 >= hunkmax)
	  if (! grow_hunkmax ())
	    {
	      p_end = -1;
	      return -1;
	    }
	sprintf (buf, "*** %s,%s\n",
		 format_linenum (numbuf0, p_first),
		 format_linenum (numbuf1, p_first + p_ptrn_lines - 1));
	p_len[0] = strlen (buf);
	if (! (p_line[0] = savestr (buf))) {
	    p_end = -1;
	    return -1;
	}
	p_Char[0] = '*';
	for (i=1; i<=p_ptrn_lines; i++) {
	    chars_read = get_line ();
	    if (chars_read == (size_t) -1)
	      {
		p_end = i - 1;
		return -1;
	      }
	    if (!chars_read)
	      fatal ("unexpected end of file in patch at line %s",
		     format_linenum (numbuf0, p_input_line));
	    if (buf[0] != '<' || (buf[1] != ' ' && buf[1] != '\t'))
	      fatal ("'<' expected at line %s of patch",
		     format_linenum (numbuf0, p_input_line));
	    chars_read -= 2 + (i == p_ptrn_lines && incomplete_line ());
	    p_len[i] = chars_read;
	    p_line[i] = savebuf (buf + 2, chars_read);
	    if (chars_read && ! p_line[i]) {
		p_end = i-1;
		return -1;
	    }
	    p_Char[i] = '-';
	}
	if (hunk_type == 'c') {
	    chars_read = get_line ();
	    if (chars_read == (size_t) -1)
	      {
		p_end = i - 1;
		return -1;
	      }
	    if (! chars_read)
	      fatal ("unexpected end of file in patch at line %s",
		     format_linenum (numbuf0, p_input_line));
	    if (*buf != '-')
	      fatal ("'---' expected at line %s of patch",
		     format_linenum (numbuf0, p_input_line));
	}
	sprintf (buf, "--- %s,%s\n",
		 format_linenum (numbuf0, min),
		 format_linenum (numbuf1, max));
	p_len[i] = strlen (buf);
	if (! (p_line[i] = savestr (buf))) {
	    p_end = i-1;
	    return -1;
	}
	p_Char[i] = '=';
	for (i++; i<=p_end; i++) {
	    chars_read = get_line ();
	    if (chars_read == (size_t) -1)
	      {
		p_end = i - 1;
		return -1;
	      }
	    if (!chars_read)
	      fatal ("unexpected end of file in patch at line %s",
		     format_linenum (numbuf0, p_input_line));
	    if (buf[0] != '>' || (buf[1] != ' ' && buf[1] != '\t'))
	      fatal ("'>' expected at line %s of patch",
		     format_linenum (numbuf0, p_input_line));
	    chars_read -= 2 + (i == p_end && incomplete_line ());
	    p_len[i] = chars_read;
	    p_line[i] = savebuf (buf + 2, chars_read);
	    if (chars_read && ! p_line[i]) {
		p_end = i-1;
		return -1;
	    }
	    p_Char[i] = '+';
	}
    }
    if (rev)				/* backwards patch? */
	if (!pch_swap())
	    say ("Not enough memory to swap next hunk!\n");
    assert (p_end + 1 < hunkmax);
    p_Char[p_end + 1] = '^';  /* add a stopper for apply_hunk */
    if (debug & 2) {
	lin i;

	for (i = 0; i <= p_end + 1; i++) {
	    fprintf (stderr, "%s %c",
		     format_linenum (numbuf0, i),
		     p_Char[i]);
	    if (p_Char[i] == '*')
	      fprintf (stderr, " %s,%s\n",
		       format_linenum (numbuf0, p_first),
		       format_linenum (numbuf1, p_ptrn_lines));
	    else if (p_Char[i] == '=')
	      fprintf (stderr, " %s,%s\n",
		       format_linenum (numbuf0, p_newfirst),
		       format_linenum (numbuf1, p_repl_lines));
	    else if (p_Char[i] != '^')
	      {
		fputs(" |", stderr);
		pch_write_line (i, stderr);
	      }
	    else
	      fputc('\n', stderr);
	}
	fflush (stderr);
    }
    return 1;
}
