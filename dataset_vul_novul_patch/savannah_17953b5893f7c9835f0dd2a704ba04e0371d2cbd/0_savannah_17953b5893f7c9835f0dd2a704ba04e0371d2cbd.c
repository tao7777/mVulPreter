main (int argc, char **argv)
{
    char const *val;
    bool somefailed = false;
    struct outstate outstate;
    struct stat tmpoutst;
    char numbuf[LINENUM_LENGTH_BOUND + 1];
    bool written_to_rejname = false;
    bool apply_empty_patch = false;
    mode_t file_type;
    int outfd = -1;
    bool have_git_diff = false;

    exit_failure = 2;
    set_program_name (argv[0]);
    init_time ();

    setbuf(stderr, serrbuf);

    bufsize = 8 * 1024;
    buf = xmalloc (bufsize);

    strippath = -1;

    val = getenv ("QUOTING_STYLE");
    {
      int i = val ? argmatch (val, quoting_style_args, 0, 0) : -1;
      set_quoting_style ((struct quoting_options *) 0,
			 i < 0 ? shell_quoting_style : (enum quoting_style) i);
    }

    posixly_correct = getenv ("POSIXLY_CORRECT") != 0;
    backup_if_mismatch = ! posixly_correct;
    patch_get = ((val = getenv ("PATCH_GET"))
		 ? numeric_string (val, true, "PATCH_GET value")
		 : 0);

    val = getenv ("SIMPLE_BACKUP_SUFFIX");
    simple_backup_suffix = val && *val ? val : ".orig";

    if ((version_control = getenv ("PATCH_VERSION_CONTROL")))
      version_control_context = "$PATCH_VERSION_CONTROL";
    else if ((version_control = getenv ("VERSION_CONTROL")))
      version_control_context = "$VERSION_CONTROL";

    init_backup_hash_table ();
    init_files_to_delete ();
    init_files_to_output ();

    /* parse switches */
    Argc = argc;
    Argv = argv;
    get_some_switches();

    /* Make get_date() assume that context diff headers use UTC. */
    if (set_utc)
      setenv ("TZ", "UTC", 1);

    if (make_backups | backup_if_mismatch)
      backup_type = get_version (version_control_context, version_control);

    init_output (&outstate);
    if (outfile)
      outstate.ofp = open_outfile (outfile);

    /* Make sure we clean up in case of disaster.  */
    set_signals (false);

    if (inname && outfile)
      {
	/* When an input and an output filename is given and the patch is
	   empty, copy the input file to the output file.  In this case, the
	   input file must be a regular file (i.e., symlinks cannot be copied
	   this way).  */
	apply_empty_patch = true;
	file_type = S_IFREG;
	inerrno = -1;
      }
    for (
	open_patch_file (patchname);
	there_is_another_patch (! (inname || posixly_correct), &file_type)
	  || apply_empty_patch;
	reinitialize_almost_everything(),
	  apply_empty_patch = false
    ) {					/* for each patch in patch file */
      int hunk = 0;
      int failed = 0;
       bool mismatch = false;
       char const *outname = NULL;
 
      if (skip_rest_of_patch)
	somefailed = true;

       if (have_git_diff != pch_git_diff ())
 	{
 	  if (have_git_diff)
	    }
	  have_git_diff = ! have_git_diff;
	}

      if (TMPREJNAME_needs_removal)
	{
	  if (rejfp)
	    {
	      fclose (rejfp);
	      rejfp = NULL;
	    }
	  remove_if_needed (TMPREJNAME, &TMPREJNAME_needs_removal);
	}
      if (TMPOUTNAME_needs_removal)
        {
	  if (outfd != -1)
	    {
	      close (outfd);
	      outfd = -1;
	    }
	  remove_if_needed (TMPOUTNAME, &TMPOUTNAME_needs_removal);
	}

      if (! skip_rest_of_patch && ! file_type)
	{
	  say ("File %s: can't change file type from 0%o to 0%o.\n",
	       quotearg (inname),
	       pch_mode (reverse) & S_IFMT,
	       pch_mode (! reverse) & S_IFMT);
	  skip_rest_of_patch = true;
	  somefailed = true;
	}

      if (! skip_rest_of_patch)
	{
	  if (outfile)
	    outname = outfile;
	  else if (pch_copy () || pch_rename ())
	    outname = pch_name (! strcmp (inname, pch_name (OLD)));
	  else
	    outname = inname;
	}

      if (pch_git_diff () && ! skip_rest_of_patch)
	{
	  struct stat outstat;
	  int outerrno = 0;

	  /* Try to recognize concatenated git diffs based on the SHA1 hashes
	     in the headers.  Will not always succeed for patches that rename
	     or copy files.  */

	  if (! strcmp (inname, outname))
	    {
	      if (inerrno == -1)
		inerrno = stat_file (inname, &instat);
	      outstat = instat;
	      outerrno = inerrno;
	    }
	  else
	    outerrno = stat_file (outname, &outstat);

	  if (! outerrno)
	    {
	      if (has_queued_output (&outstat))
		{
		  output_files (&outstat);
		  outerrno = stat_file (outname, &outstat);
		  inerrno = -1;
		}
	      if (! outerrno)
		set_queued_output (&outstat, true);
	    }
	}

      if (! skip_rest_of_patch)
	{
	  if (! get_input_file (inname, outname, file_type))
	    {
	      skip_rest_of_patch = true;
	      somefailed = true;
	    }
	}

      if (read_only_behavior != RO_IGNORE
	  && ! inerrno && ! S_ISLNK (instat.st_mode)
	  && access (inname, W_OK) != 0)
	{
	  say ("File %s is read-only; ", quotearg (inname));
	  if (read_only_behavior == RO_WARN)
	    say ("trying to patch anyway\n");
	  else
	    {
	      say ("refusing to patch\n");
	      skip_rest_of_patch = true;
	      somefailed = true;
	    }
	}

      tmpoutst.st_size = -1;
      outfd = make_tempfile (&TMPOUTNAME, 'o', outname,
			     O_WRONLY | binary_transput,
			     instat.st_mode & S_IRWXUGO);
      TMPOUTNAME_needs_removal = true;
      if (diff_type == ED_DIFF) {
	outstate.zero_output = false;
	somefailed |= skip_rest_of_patch;
	do_ed_script (inname, TMPOUTNAME, &TMPOUTNAME_needs_removal,
		      outstate.ofp);
	if (! dry_run && ! outfile && ! skip_rest_of_patch)
	  {
	    if (fstat (outfd, &tmpoutst) != 0)
	      pfatal ("%s", TMPOUTNAME);
	    outstate.zero_output = tmpoutst.st_size == 0;
	  }
	close (outfd);
	outfd = -1;
      } else {
	int got_hunk;
	bool apply_anyway = merge;  /* don't try to reverse when merging */

	if (! skip_rest_of_patch && diff_type == GIT_BINARY_DIFF) {
	  say ("File %s: git binary diffs are not supported.\n",
	       quotearg (outname));
	  skip_rest_of_patch = true;
	  somefailed = true;
	}
	/* initialize the patched file */
	if (! skip_rest_of_patch && ! outfile)
	  {
	    init_output (&outstate);
	    outstate.ofp = fdopen(outfd, binary_transput ? "wb" : "w");
	    if (! outstate.ofp)
	      pfatal ("%s", TMPOUTNAME);
	  }

	/* find out where all the lines are */
	if (!skip_rest_of_patch) {
	    scan_input (inname, file_type);

	    if (verbosity != SILENT)
	      {
		bool renamed = strcmp (inname, outname);

		say ("%s %s %s%c",
		     dry_run ? "checking" : "patching",
		     S_ISLNK (file_type) ? "symbolic link" : "file",
		     quotearg (outname), renamed ? ' ' : '\n');
		if (renamed)
		  say ("(%s from %s)\n",
		       pch_copy () ? "copied" :
		       (pch_rename () ? "renamed" : "read"),
		       inname);
		if (verbosity == VERBOSE)
		  say ("Using Plan %s...\n", using_plan_a ? "A" : "B");
	      }
	}

	/* from here on, open no standard i/o files, because malloc */
	/* might misfire and we can't catch it easily */

	/* apply each hunk of patch */
	while (0 < (got_hunk = another_hunk (diff_type, reverse)))
	  {
	    lin where = 0; /* Pacify 'gcc -Wall'.  */
	    lin newwhere;
	    lin fuzz = 0;
	    lin mymaxfuzz;

	    if (merge)
	      {
		/* When in merge mode, don't apply with fuzz.  */
		mymaxfuzz = 0;
	      }
	    else
	      {
		lin prefix_context = pch_prefix_context ();
		lin suffix_context = pch_suffix_context ();
		lin context = (prefix_context < suffix_context
			       ? suffix_context : prefix_context);
		mymaxfuzz = (maxfuzz < context ? maxfuzz : context);
	      }

	    hunk++;
	    if (!skip_rest_of_patch) {
		do {
		    where = locate_hunk(fuzz);
		    if (! where || fuzz || in_offset)
		      mismatch = true;
		    if (hunk == 1 && ! where && ! (force | apply_anyway)
			&& reverse == reverse_flag_specified) {
						/* dwim for reversed patch? */
			if (!pch_swap()) {
			    say (
"Not enough memory to try swapped hunk!  Assuming unswapped.\n");
			    continue;
			}
			/* Try again.  */
			where = locate_hunk (fuzz);
			if (where
			    && (ok_to_reverse
				("%s patch detected!",
				 (reverse
				  ? "Unreversed"
				  : "Reversed (or previously applied)"))))
			  reverse = ! reverse;
			else
			  {
			    /* Put it back to normal.  */
			    if (! pch_swap ())
			      fatal ("lost hunk on alloc error!");
			    if (where)
			      {
				apply_anyway = true;
				fuzz--; /* Undo '++fuzz' below.  */
				where = 0;
			      }
			  }
		    }
		} while (!skip_rest_of_patch && !where
			 && ++fuzz <= mymaxfuzz);

		if (skip_rest_of_patch) {		/* just got decided */
		  if (outstate.ofp && ! outfile)
		    {
		      fclose (outstate.ofp);
		      outstate.ofp = 0;
		      outfd = -1;
		    }
		}
	    }

	    newwhere = (where ? where : pch_first()) + out_offset;
	    if (skip_rest_of_patch
		|| (merge && ! merge_hunk (hunk, &outstate, where,
					   &somefailed))
		|| (! merge
		    && ((where == 1 && pch_says_nonexistent (reverse) == 2
			 && instat.st_size)
			|| ! where
			|| ! apply_hunk (&outstate, where))))
	      {
		abort_hunk (outname, ! failed, reverse);
		failed++;
		if (verbosity == VERBOSE ||
		    (! skip_rest_of_patch && verbosity != SILENT))
		  say ("Hunk #%d %s at %s%s.\n", hunk,
		       skip_rest_of_patch ? "ignored" : "FAILED",
		       format_linenum (numbuf, newwhere),
		       ! skip_rest_of_patch && check_line_endings (newwhere)
			 ?  " (different line endings)" : "");
	      }
	    else if (! merge &&
		     (verbosity == VERBOSE
		      || (verbosity != SILENT && (fuzz || in_offset))))
	      {
		say ("Hunk #%d succeeded at %s", hunk,
		     format_linenum (numbuf, newwhere));
		if (fuzz)
		  say (" with fuzz %s", format_linenum (numbuf, fuzz));
		if (in_offset)
		  say (" (offset %s line%s)",
		       format_linenum (numbuf, in_offset),
		       "s" + (in_offset == 1));
		say (".\n");
	      }
	  }

	if (!skip_rest_of_patch)
	  {
	    if (got_hunk < 0  &&  using_plan_a)
	      {
		if (outfile)
		  fatal ("out of memory using Plan A");
		say ("\n\nRan out of memory using Plan A -- trying again...\n\n");
		if (outstate.ofp)
		  {
		    fclose (outstate.ofp);
		    outstate.ofp = 0;
		  }
		continue;
	      }

	    /* Finish spewing out the new file.  */
	    if (! spew_output (&outstate, &tmpoutst))
	      {
		say ("Skipping patch.\n");
		skip_rest_of_patch = true;
	      }
	  }
      }

      /* and put the output where desired */
      ignore_signals ();
      if (! skip_rest_of_patch && ! outfile) {
	  bool backup = make_backups
			|| (backup_if_mismatch && (mismatch | failed));
	  if (outstate.zero_output
	      && (remove_empty_files
		  || (pch_says_nonexistent (! reverse) == 2
		      && ! posixly_correct)
		  || S_ISLNK (file_type)))
	    {
	      if (! dry_run)
		output_file (NULL, NULL, NULL, outname,
			     (inname == outname) ? &instat : NULL,
			     file_type | 0, backup);
	    }
	  else
	    {
	      if (! outstate.zero_output
		  && pch_says_nonexistent (! reverse) == 2
		  && (remove_empty_files || ! posixly_correct)
		  && ! (merge && somefailed))
		{
		  mismatch = true;
		  somefailed = true;
		  if (verbosity != SILENT)
		    say ("Not deleting file %s as content differs from patch\n",
			 quotearg (outname));
		}

	      if (! dry_run)
		{
		  mode_t old_mode = pch_mode (reverse);
		  mode_t new_mode = pch_mode (! reverse);
		  bool set_mode = new_mode && old_mode != new_mode;

		  /* Avoid replacing files when nothing has changed.  */
		  if (failed < hunk || diff_type == ED_DIFF || set_mode
		      || pch_copy () || pch_rename ())
		    {
		      enum file_attributes attr = 0;
		      struct timespec new_time = pch_timestamp (! reverse);
		      mode_t mode = file_type |
			  ((new_mode ? new_mode : instat.st_mode) & S_IRWXUGO);

		      if ((set_time | set_utc) && new_time.tv_sec != -1)
			{
			  struct timespec old_time = pch_timestamp (reverse);

			  if (! force && ! inerrno
			      && pch_says_nonexistent (reverse) != 2
			      && old_time.tv_sec != -1
			      && timespec_cmp (old_time,
					       get_stat_mtime (&instat)))
			    say ("Not setting time of file %s "
				 "(time mismatch)\n",
				 quotearg (outname));
			  else if (! force && (mismatch | failed))
			    say ("Not setting time of file %s "
				 "(contents mismatch)\n",
				 quotearg (outname));
			  else
			    attr |= FA_TIMES;
			}

		      if (inerrno)
			set_file_attributes (TMPOUTNAME, attr, NULL, NULL,
					     mode, &new_time);
		      else
			{
			  attr |= FA_IDS | FA_MODE | FA_XATTRS;
			  set_file_attributes (TMPOUTNAME, attr, inname, &instat,
					       mode, &new_time);
			}

		      output_file (TMPOUTNAME, &TMPOUTNAME_needs_removal,
				   &tmpoutst, outname, NULL, mode, backup);

		      if (pch_rename ())
			output_file (NULL, NULL, NULL, inname, &instat,
				     mode, backup);
		    }
		  else
		    output_file (outname, NULL, &tmpoutst, NULL, NULL,
				 file_type | 0, backup);
		}
	    }
      }
      if (diff_type != ED_DIFF) {
	struct stat rejst;

	if (failed) {
	    if (fstat (fileno (rejfp), &rejst) != 0 || fclose (rejfp) != 0)
	      write_fatal ();
	    rejfp = NULL;
	    somefailed = true;
	    say ("%d out of %d hunk%s %s", failed, hunk, "s" + (hunk == 1),
		 skip_rest_of_patch ? "ignored" : "FAILED");
	    if (outname && (! rejname || strcmp (rejname, "-") != 0)) {
		char *rej = rejname;
		if (!rejname) {
		    /* FIXME: This should really be done differently!  */
		    const char *s = simple_backup_suffix;
		    size_t len;
		    simple_backup_suffix = ".rej";
		    rej = find_backup_file_name (outname, simple_backups);
		    len = strlen (rej);
		    if (rej[len - 1] == '~')
		      rej[len - 1] = '#';
		    simple_backup_suffix = s;
		}
		if (! dry_run)
		  {
		    say (" -- saving rejects to file %s\n", quotearg (rej));
		    if (rejname)
		      {
			if (! written_to_rejname)
			  {
			    copy_file (TMPREJNAME, rejname, 0, 0,
				       S_IFREG | 0666, true);
			    written_to_rejname = true;
			  }
			else
			  append_to_file (TMPREJNAME, rejname);
		      }
		    else
		      {
			struct stat oldst;
			int olderrno;

			olderrno = stat_file (rej, &oldst);
			if (olderrno && olderrno != ENOENT)
			  write_fatal ();
		        if (! olderrno && lookup_file_id (&oldst) == CREATED)
			  append_to_file (TMPREJNAME, rej);
			else
			  move_file (TMPREJNAME, &TMPREJNAME_needs_removal,
				     &rejst, rej, S_IFREG | 0666, false);
		      }
		  }
		else
		  say ("\n");
		if (!rejname)
		    free (rej);
	    } else
	      say ("\n");
	}
      }
      set_signals (true);
    }
