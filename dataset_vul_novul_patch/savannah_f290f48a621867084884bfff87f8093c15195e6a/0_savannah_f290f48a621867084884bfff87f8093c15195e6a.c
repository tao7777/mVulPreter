intuit_diff_type (bool need_header, mode_t *p_file_type)
{
    file_offset this_line = 0;
    file_offset first_command_line = -1;
    char first_ed_command_letter = 0;
    lin fcl_line = 0; /* Pacify 'gcc -W'.  */
    bool this_is_a_command = false;
    bool stars_this_line = false;
    bool extended_headers = false;
    enum nametype i;
    struct stat st[3];
    int stat_errno[3];
    int version_controlled[3];
    enum diff retval;
    mode_t file_type;
    size_t indent = 0;

    for (i = OLD;  i <= INDEX;  i++)
      if (p_name[i]) {
	  free (p_name[i]);
	  p_name[i] = 0;
        }
    for (i = 0; i < ARRAY_SIZE (invalid_names); i++)
	invalid_names[i] = NULL;
    for (i = OLD; i <= NEW; i++)
      if (p_timestr[i])
	{
	  free(p_timestr[i]);
	  p_timestr[i] = 0;
	}
    for (i = OLD; i <= NEW; i++)
      if (p_sha1[i])
	{
	  free (p_sha1[i]);
	  p_sha1[i] = 0;
	}
    p_git_diff = false;
    for (i = OLD; i <= NEW; i++)
      {
	p_mode[i] = 0;
	p_copy[i] = false;
	p_rename[i] = false;
      }

    /* Ed and normal format patches don't have filename headers.  */
    if (diff_type == ED_DIFF || diff_type == NORMAL_DIFF)
      need_header = false;

    version_controlled[OLD] = -1;
    version_controlled[NEW] = -1;
    version_controlled[INDEX] = -1;
    p_rfc934_nesting = 0;
    p_timestamp[OLD].tv_sec = p_timestamp[NEW].tv_sec = -1;
    p_says_nonexistent[OLD] = p_says_nonexistent[NEW] = 0;
    Fseek (pfp, p_base, SEEK_SET);
    p_input_line = p_bline - 1;
    for (;;) {
	char *s;
	char *t;
	file_offset previous_line = this_line;
	bool last_line_was_command = this_is_a_command;
	bool stars_last_line = stars_this_line;
	size_t indent_last_line = indent;
	char ed_command_letter;
	bool strip_trailing_cr;
	size_t chars_read;

	indent = 0;
	this_line = file_tell (pfp);
	chars_read = pget_line (0, 0, false, false);
	if (chars_read == (size_t) -1)
	  xalloc_die ();
	if (! chars_read) {
	    if (first_ed_command_letter) {
					/* nothing but deletes!? */
		p_start = first_command_line;
		p_sline = fcl_line;
		retval = ED_DIFF;
		goto scan_exit;
	    }
	    else {
		p_start = this_line;
		p_sline = p_input_line;
		if (extended_headers)
		  {
		    /* Patch contains no hunks; any diff type will do. */
		    retval = UNI_DIFF;
		    goto scan_exit;
		  }
		return NO_DIFF;
	    }
	}
	strip_trailing_cr = 2 <= chars_read && buf[chars_read - 2] == '\r';
	for (s = buf; *s == ' ' || *s == '\t' || *s == 'X'; s++) {
	    if (*s == '\t')
		indent = (indent + 8) & ~7;
	    else
		indent++;
	}
	if (ISDIGIT (*s))
	  {
	    for (t = s + 1; ISDIGIT (*t) || *t == ',';  t++)
	      /* do nothing */ ;
	    if (*t == 'd' || *t == 'c' || *t == 'a')
	      {
		for (t++;  ISDIGIT (*t) || *t == ',';  t++)
		  /* do nothing */ ;
		for (; *t == ' ' || *t == '\t'; t++)
		  /* do nothing */ ;
		if (*t == '\r')
		  t++;
		this_is_a_command = (*t == '\n');
	      }
	  }
	if (! need_header
	    && first_command_line < 0
	    && ((ed_command_letter = get_ed_command_letter (s))
		|| this_is_a_command)) {
	    first_command_line = this_line;
	    first_ed_command_letter = ed_command_letter;
	    fcl_line = p_input_line;
	    p_indent = indent;		/* assume this for now */
	    p_strip_trailing_cr = strip_trailing_cr;
	}
	if (!stars_last_line && strnEQ(s, "*** ", 4))
	  {
	    fetchname (s+4, strippath, &p_name[OLD], &p_timestr[OLD],
		       &p_timestamp[OLD]);
	    need_header = false;
	  }
	else if (strnEQ(s, "+++ ", 4))
	  {
	    /* Swap with NEW below.  */
	    fetchname (s+4, strippath, &p_name[OLD], &p_timestr[OLD],
		       &p_timestamp[OLD]);
	    need_header = false;
	    p_strip_trailing_cr = strip_trailing_cr;
	  }
	else if (strnEQ(s, "Index:", 6))
	  {
	    fetchname (s+6, strippath, &p_name[INDEX], (char **) 0, NULL);
	    need_header = false;
	    p_strip_trailing_cr = strip_trailing_cr;
	  }
	else if (strnEQ(s, "Prereq:", 7))
	  {
	    for (t = s + 7;  ISSPACE ((unsigned char) *t);  t++)
	      /* do nothing */ ;
	    revision = t;
	    for (t = revision;  *t;  t++)
	      if (ISSPACE ((unsigned char) *t))
		{
		  char const *u;
		  for (u = t + 1;  ISSPACE ((unsigned char) *u);  u++)
		    /* do nothing */ ;
		  if (*u)
		    {
		      char numbuf[LINENUM_LENGTH_BOUND + 1];
		      say ("Prereq: with multiple words at line %s of patch\n",
			   format_linenum (numbuf, this_line));
		    }
		  break;
		}
	    if (t == revision)
		revision = 0;
	    else {
		char oldc = *t;
		*t = '\0';
		revision = xstrdup (revision);
		*t = oldc;
	    }
	  }
	else if (strnEQ (s, "diff --git ", 11))
	  {
	    char const *u;

	    if (extended_headers)
	      {
		p_start = this_line;
		p_sline = p_input_line;
		/* Patch contains no hunks; any diff type will do. */
		retval = UNI_DIFF;
		goto scan_exit;
	      }

	    for (i = OLD; i <= NEW; i++)
	      {
		free (p_name[i]);
		p_name[i] = 0;
	      }
	    if (! ((p_name[OLD] = parse_name (s + 11, strippath, &u))
		   && ISSPACE ((unsigned char) *u)
		   && (p_name[NEW] = parse_name (u, strippath, &u))
		   && (u = skip_spaces (u), ! *u)))
	      for (i = OLD; i <= NEW; i++)
		{
		  free (p_name[i]);
		  p_name[i] = 0;
		}
	    p_git_diff = true;
	    need_header = false;
	  }
	else if (p_git_diff && strnEQ (s, "index ", 6))
	  {
	    char const *u, *v;
	    if ((u = skip_hex_digits (s + 6))
		&& u[0] == '.' && u[1] == '.'
		&& (v = skip_hex_digits (u + 2))
		&& (! *v || ISSPACE ((unsigned char) *v)))
	      {
		get_sha1(&p_sha1[OLD], s + 6, u);
		get_sha1(&p_sha1[NEW], u + 2, v);
		p_says_nonexistent[OLD] = sha1_says_nonexistent (p_sha1[OLD]);
		p_says_nonexistent[NEW] = sha1_says_nonexistent (p_sha1[NEW]);
		if (*(v = skip_spaces (v)))
		  p_mode[OLD] = p_mode[NEW] = fetchmode (v);
		extended_headers = true;
	      }
	  }
	else if (p_git_diff && strnEQ (s, "old mode ", 9))
	  {
	    p_mode[OLD] = fetchmode (s + 9);
	    extended_headers = true;
	  }
	else if (p_git_diff && strnEQ (s, "new mode ", 9))
	  {
	    p_mode[NEW] = fetchmode (s + 9);
	    extended_headers = true;
	  }
	else if (p_git_diff && strnEQ (s, "deleted file mode ", 18))
	  {
	    p_mode[OLD] = fetchmode (s + 18);
	    p_says_nonexistent[NEW] = 2;
	    extended_headers = true;
	  }
	else if (p_git_diff && strnEQ (s, "new file mode ", 14))
	  {
	    p_mode[NEW] = fetchmode (s + 14);
	    p_says_nonexistent[OLD] = 2;
	    extended_headers = true;
	  }
	else if (p_git_diff && strnEQ (s, "rename from ", 12))
	  {
	    /* Git leaves out the prefix in the file name in this header,
	       so we can only ignore the file name.  */
	    p_rename[OLD] = true;
	    extended_headers = true;
	  }
	else if (p_git_diff && strnEQ (s, "rename to ", 10))
	  {
	    /* Git leaves out the prefix in the file name in this header,
	       so we can only ignore the file name.  */
	    p_rename[NEW] = true;
	    extended_headers = true;
	  }
	else if (p_git_diff && strnEQ (s, "copy from ", 10))
	  {
	    /* Git leaves out the prefix in the file name in this header,
	       so we can only ignore the file name.  */
	    p_copy[OLD] = true;
	    extended_headers = true;
	  }
	else if (p_git_diff && strnEQ (s, "copy to ", 8))
	  {
	    /* Git leaves out the prefix in the file name in this header,
	       so we can only ignore the file name.  */
	    p_copy[NEW] = true;
	    extended_headers = true;
	  }
	else if (p_git_diff && strnEQ (s, "GIT binary patch", 16))
	  {
	    p_start = this_line;
	    p_sline = p_input_line;
	    retval = GIT_BINARY_DIFF;
	    goto scan_exit;
	  }
	else
	  {
	    for (t = s;  t[0] == '-' && t[1] == ' ';  t += 2)
	      /* do nothing */ ;
	    if (strnEQ(t, "--- ", 4))
	      {
		struct timespec timestamp;
		timestamp.tv_sec = -1;
		fetchname (t+4, strippath, &p_name[NEW], &p_timestr[NEW],
			   &timestamp);
		need_header = false;
		if (timestamp.tv_sec != -1)
		  {
		    p_timestamp[NEW] = timestamp;
		    p_rfc934_nesting = (t - s) >> 1;
		  }
		p_strip_trailing_cr = strip_trailing_cr;
	      }
	  }
	if (need_header)
	  continue;
	if ((diff_type == NO_DIFF || diff_type == ED_DIFF) &&
	  first_command_line >= 0 &&
	  strEQ(s, ".\n") ) {
	    p_start = first_command_line;
	    p_sline = fcl_line;
	    retval = ED_DIFF;
	    goto scan_exit;
	}
	if ((diff_type == NO_DIFF || diff_type == UNI_DIFF)
	    && strnEQ(s, "@@ -", 4)) {

	    /* 'p_name', 'p_timestr', and 'p_timestamp' are backwards;
	       swap them.  */
	    struct timespec ti = p_timestamp[OLD];
	    p_timestamp[OLD] = p_timestamp[NEW];
	    p_timestamp[NEW] = ti;
	    t = p_name[OLD];
	    p_name[OLD] = p_name[NEW];
	    p_name[NEW] = t;
	    t = p_timestr[OLD];
	    p_timestr[OLD] = p_timestr[NEW];
	    p_timestr[NEW] = t;

	    s += 4;
	    if (s[0] == '0' && !ISDIGIT (s[1]))
	      p_says_nonexistent[OLD] = 1 + ! p_timestamp[OLD].tv_sec;
	    while (*s != ' ' && *s != '\n')
	      s++;
	    while (*s == ' ')
	      s++;
	    if (s[0] == '+' && s[1] == '0' && !ISDIGIT (s[2]))
	      p_says_nonexistent[NEW] = 1 + ! p_timestamp[NEW].tv_sec;
	    p_indent = indent;
	    p_start = this_line;
	    p_sline = p_input_line;
	    retval = UNI_DIFF;
	    if (! ((p_name[OLD] || ! p_timestamp[OLD].tv_sec)
		   && (p_name[NEW] || ! p_timestamp[NEW].tv_sec))
		&& ! p_name[INDEX] && need_header)
	      {
		char numbuf[LINENUM_LENGTH_BOUND + 1];
		say ("missing header for unified diff at line %s of patch\n",
		     format_linenum (numbuf, p_sline));
	      }
	    goto scan_exit;
	}
	stars_this_line = strnEQ(s, "********", 8);
	if ((diff_type == NO_DIFF
	     || diff_type == CONTEXT_DIFF
	     || diff_type == NEW_CONTEXT_DIFF)
	    && stars_last_line && indent_last_line == indent
	    && strnEQ (s, "*** ", 4)) {
	    s += 4;
	    if (s[0] == '0' && !ISDIGIT (s[1]))
	      p_says_nonexistent[OLD] = 1 + ! p_timestamp[OLD].tv_sec;
	    /* if this is a new context diff the character just before */
	    /* the newline is a '*'. */
	    while (*s != '\n')
		s++;
	    p_indent = indent;
	    p_strip_trailing_cr = strip_trailing_cr;
	    p_start = previous_line;
	    p_sline = p_input_line - 1;
	    retval = (*(s-1) == '*' ? NEW_CONTEXT_DIFF : CONTEXT_DIFF);

	    {
	      /* Scan the first hunk to see whether the file contents
		 appear to have been deleted.  */
	      file_offset saved_p_base = p_base;
	      lin saved_p_bline = p_bline;
	      Fseek (pfp, previous_line, SEEK_SET);
	      p_input_line -= 2;
	      if (another_hunk (retval, false)
		  && ! p_repl_lines && p_newfirst == 1)
		p_says_nonexistent[NEW] = 1 + ! p_timestamp[NEW].tv_sec;
	      next_intuit_at (saved_p_base, saved_p_bline);
	    }

	    if (! ((p_name[OLD] || ! p_timestamp[OLD].tv_sec)
		   && (p_name[NEW] || ! p_timestamp[NEW].tv_sec))
		&& ! p_name[INDEX] && need_header)
	      {
		char numbuf[LINENUM_LENGTH_BOUND + 1];
		say ("missing header for context diff at line %s of patch\n",
		     format_linenum (numbuf, p_sline));
	      }
	    goto scan_exit;
	}
	if ((diff_type == NO_DIFF || diff_type == NORMAL_DIFF) &&
	  last_line_was_command &&
	  (strnEQ(s, "< ", 2) || strnEQ(s, "> ", 2)) ) {
	    p_start = previous_line;
	    p_sline = p_input_line - 1;
	    p_indent = indent;
	    retval = NORMAL_DIFF;
	    goto scan_exit;
	}
    }

  scan_exit:

    /* The old, new, or old and new file types may be defined.  When both
       file types are defined, make sure they are the same, or else assume
       we do not know the file type.  */
    file_type = p_mode[OLD] & S_IFMT;
    if (file_type)
      {
	mode_t new_file_type = p_mode[NEW] & S_IFMT;
	if (new_file_type && file_type != new_file_type)
	  file_type = 0;
      }
    else
      {
	file_type = p_mode[NEW] & S_IFMT;
	if (! file_type)
	  file_type = S_IFREG;
      }
    *p_file_type = file_type;

    /* To intuit 'inname', the name of the file to patch,
       use the algorithm specified by POSIX 1003.1-2001 XCU lines 25680-26599
       (with some modifications if posixly_correct is zero):

       - Take the old and new names from the context header if present,
	 and take the index name from the 'Index:' line if present and
	 if either the old and new names are both absent
	 or posixly_correct is nonzero.
	 Consider the file names to be in the order (old, new, index).
       - If some named files exist, use the first one if posixly_correct
	 is nonzero, the best one otherwise.
       - If patch_get is nonzero, and no named files exist,
	 but an RCS or SCCS master file exists,
	 use the first named file with an RCS or SCCS master.
       - If no named files exist, no RCS or SCCS master was found,
	 some names are given, posixly_correct is zero,
	 and the patch appears to create a file, then use the best name
	 requiring the creation of the fewest directories.
       - Otherwise, report failure by setting 'inname' to 0;
	 this causes our invoker to ask the user for a file name.  */

    i = NONE;

    if (!inname)
      {
	enum nametype i0 = NONE;

	if (! posixly_correct && (p_name[OLD] || p_name[NEW]) && p_name[INDEX])
	  {
	    free (p_name[INDEX]);
	    p_name[INDEX] = 0;
	  }

	for (i = OLD;  i <= INDEX;  i++)
	  if (p_name[i])
	    {
	      if (i0 != NONE && strcmp (p_name[i0], p_name[i]) == 0)
		{
		  /* It's the same name as before; reuse stat results.  */
		  stat_errno[i] = stat_errno[i0];
		  if (! stat_errno[i])
		    st[i] = st[i0];
		}
	      else
		{
		  stat_errno[i] = stat_file (p_name[i], &st[i]);
		  if (! stat_errno[i])
		    {
		      if (lookup_file_id (&st[i]) == DELETE_LATER)
			stat_errno[i] = ENOENT;
		      else if (posixly_correct && name_is_valid (p_name[i]))
			break;
		    }
		}
	      i0 = i;
	    }

	if (! posixly_correct)
	  {
	    /* The best of all existing files. */
	    i = best_name (p_name, stat_errno);

	    if (i == NONE && patch_get)
	      {
		enum nametype nope = NONE;

		for (i = OLD;  i <= INDEX;  i++)
		  if (p_name[i])
		    {
		      char const *cs;
		      char *getbuf;
		      char *diffbuf;
		      bool readonly = (outfile
				       && strcmp (outfile, p_name[i]) != 0);

		      if (nope == NONE || strcmp (p_name[nope], p_name[i]) != 0)
			{
			  cs = (version_controller
			        (p_name[i], readonly, (struct stat *) 0,
				 &getbuf, &diffbuf));
			  version_controlled[i] = !! cs;
			  if (cs)
			    {
			      if (version_get (p_name[i], cs, false, readonly,
					       getbuf, &st[i]))
				stat_errno[i] = 0;
			      else
				version_controlled[i] = 0;

			      free (getbuf);
			      free (diffbuf);

			      if (! stat_errno[i])
				break;
			    }
			}

		      nope = i;
		    }
	      }

	    if (i0 != NONE
		&& (i == NONE || (st[i].st_mode & S_IFMT) == file_type)
		&& maybe_reverse (p_name[i == NONE ? i0 : i], i == NONE,
				  i == NONE || st[i].st_size == 0)
		&& i == NONE)
	      i = i0;

	    if (i == NONE && p_says_nonexistent[reverse])
	      {
		int newdirs[3];
		int newdirs_min = INT_MAX;
		int distance_from_minimum[3];

		for (i = OLD;  i <= INDEX;  i++)
		  if (p_name[i])
		    {
		      newdirs[i] = (prefix_components (p_name[i], false)
				    - prefix_components (p_name[i], true));
		      if (newdirs[i] < newdirs_min)
			newdirs_min = newdirs[i];
		    }

		for (i = OLD;  i <= INDEX;  i++)
		  if (p_name[i])
		    distance_from_minimum[i] = newdirs[i] - newdirs_min;

		/* The best of the filenames which create the fewest directories. */
		i = best_name (p_name, distance_from_minimum);
	      }
	  }
      }

     if ((pch_rename () || pch_copy ())
 	&& ! inname
 	&& ! ((i == OLD || i == NEW) &&
	      p_name[reverse] && p_name[! reverse] &&
	      name_is_valid (p_name[reverse]) &&
 	      name_is_valid (p_name[! reverse])))
       {
 	say ("Cannot %s file without two valid file names\n", pch_rename () ? "rename" : "copy");
      }

    if (i == NONE)
      {
	if (inname)
	  {
	    inerrno = stat_file (inname, &instat);
	    if (inerrno || (instat.st_mode & S_IFMT) == file_type)
	      maybe_reverse (inname, inerrno, inerrno || instat.st_size == 0);
	  }
	else
          inerrno = -1;
      }
    else
      {
	inname = xstrdup (p_name[i]);
	inerrno = stat_errno[i];
	invc = version_controlled[i];
	instat = st[i];
      }

    return retval;
}
