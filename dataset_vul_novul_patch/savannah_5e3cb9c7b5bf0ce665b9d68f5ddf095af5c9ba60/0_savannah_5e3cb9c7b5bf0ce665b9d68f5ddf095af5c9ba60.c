main (int argc, char *argv[])
{
  struct gengetopt_args_info args_info;
  char *line = NULL;
  size_t linelen = 0;
  char *p, *r;
  uint32_t *q;
  unsigned cmdn = 0;
  int rc;

  setlocale (LC_ALL, "");
  set_program_name (argv[0]);
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  if (cmdline_parser (argc, argv, &args_info) != 0)
    return EXIT_FAILURE;

  if (args_info.version_given)
    {
      version_etc (stdout, "idn", PACKAGE_NAME, VERSION,
		   "Simon Josefsson", (char *) NULL);
      return EXIT_SUCCESS;
    }

  if (args_info.help_given)
    usage (EXIT_SUCCESS);

  /* Backwards compatibility: -n has always been the documented short
     form for --nfkc but, before v1.10, -k was the implemented short
     form.  We now accept both to avoid documentation changes.  */
  if (args_info.hidden_nfkc_given)
    args_info.nfkc_given = 1;

  if (!args_info.stringprep_given &&
      !args_info.punycode_encode_given && !args_info.punycode_decode_given &&
      !args_info.idna_to_ascii_given && !args_info.idna_to_unicode_given &&
      !args_info.nfkc_given)
    args_info.idna_to_ascii_given = 1;

  if ((args_info.stringprep_given ? 1 : 0) +
      (args_info.punycode_encode_given ? 1 : 0) +
      (args_info.punycode_decode_given ? 1 : 0) +
      (args_info.idna_to_ascii_given ? 1 : 0) +
      (args_info.idna_to_unicode_given ? 1 : 0) +
      (args_info.nfkc_given ? 1 : 0) != 1)
    {
      error (0, 0, _("only one of -s, -e, -d, -a, -u or -n can be specified"));
      usage (EXIT_FAILURE);
    }

  if (!args_info.quiet_given
      && args_info.inputs_num == 0
      && isatty (fileno (stdin)))
    fprintf (stderr, "%s %s\n" GREETING, PACKAGE, VERSION);

  if (args_info.debug_given)
    fprintf (stderr, _("Charset `%s'.\n"), stringprep_locale_charset ());

  if (!args_info.quiet_given
      && args_info.inputs_num == 0
      && isatty (fileno (stdin)))
    fprintf (stderr, _("Type each input string on a line by itself, "
		       "terminated by a newline character.\n"));

  do
    {
      if (cmdn < args_info.inputs_num)
	line = strdup (args_info.inputs[cmdn++]);
      else if (getline (&line, &linelen, stdin) == -1)
	{
	  if (feof (stdin))
	    break;

 	  error (EXIT_FAILURE, errno, _("input error"));
 	}
 
      if (strlen (line) > 0)
	if (line[strlen (line) - 1] == '\n')
	  line[strlen (line) - 1] = '\0';
 
       if (args_info.stringprep_given)
 	{
	  if (!p)
	    error (EXIT_FAILURE, 0, _("could not convert from %s to UTF-8"),
		   stringprep_locale_charset ());

	  q = stringprep_utf8_to_ucs4 (p, -1, NULL);
	  if (!q)
	    {
	      free (p);
	      error (EXIT_FAILURE, 0,
		     _("could not convert from UTF-8 to UCS-4"));
	    }

	  if (args_info.debug_given)
	    {
	      size_t i;
	      for (i = 0; q[i]; i++)
		fprintf (stderr, "input[%lu] = U+%04x\n",
			 (unsigned long) i, q[i]);
	    }
	  free (q);

	  rc = stringprep_profile (p, &r,
				   args_info.profile_given ?
				   args_info.profile_arg : "Nameprep", 0);
	  free (p);
	  if (rc != STRINGPREP_OK)
	    error (EXIT_FAILURE, 0, _("stringprep_profile: %s"),
		   stringprep_strerror (rc));

	  q = stringprep_utf8_to_ucs4 (r, -1, NULL);
	  if (!q)
	    {
	      free (r);
	      error (EXIT_FAILURE, 0,
		     _("could not convert from UTF-8 to UCS-4"));
	    }

	  if (args_info.debug_given)
	    {
	      size_t i;
	      for (i = 0; q[i]; i++)
		fprintf (stderr, "output[%lu] = U+%04x\n",
			 (unsigned long) i, q[i]);
	    }
	  free (q);

	  p = stringprep_utf8_to_locale (r);
	  free (r);
	  if (!p)
	    error (EXIT_FAILURE, 0, _("could not convert from UTF-8 to %s"),
		   stringprep_locale_charset ());

	  fprintf (stdout, "%s\n", p);

	  free (p);
	}

      if (args_info.punycode_encode_given)
	{
	  char encbuf[BUFSIZ];
	  size_t len, len2;

	  p = stringprep_locale_to_utf8 (line);
	  if (!p)
	    error (EXIT_FAILURE, 0, _("could not convert from %s to UTF-8"),
		   stringprep_locale_charset ());

	  q = stringprep_utf8_to_ucs4 (p, -1, &len);
	  free (p);
	  if (!q)
	    error (EXIT_FAILURE, 0,
		   _("could not convert from UTF-8 to UCS-4"));

	  if (args_info.debug_given)
	    {
	      size_t i;
	      for (i = 0; i < len; i++)
		fprintf (stderr, "input[%lu] = U+%04x\n",
			 (unsigned long) i, q[i]);
	    }

	  len2 = BUFSIZ - 1;
	  rc = punycode_encode (len, q, NULL, &len2, encbuf);
	  free (q);
	  if (rc != PUNYCODE_SUCCESS)
	    error (EXIT_FAILURE, 0, _("punycode_encode: %s"),
		   punycode_strerror (rc));

	  encbuf[len2] = '\0';

	  p = stringprep_utf8_to_locale (encbuf);
	  if (!p)
	    error (EXIT_FAILURE, 0, _("could not convert from UTF-8 to %s"),
		   stringprep_locale_charset ());

	  fprintf (stdout, "%s\n", p);

	  free (p);
	}

      if (args_info.punycode_decode_given)
	{
	  size_t len;

	  len = BUFSIZ;
	  q = (uint32_t *) malloc (len * sizeof (q[0]));
	  if (!q)
	    error (EXIT_FAILURE, ENOMEM, N_("malloc"));

	  rc = punycode_decode (strlen (line), line, &len, q, NULL);
	  if (rc != PUNYCODE_SUCCESS)
	    {
	      free (q);
	      error (EXIT_FAILURE, 0, _("punycode_decode: %s"),
		     punycode_strerror (rc));
	    }

	  if (args_info.debug_given)
	    {
	      size_t i;
	      for (i = 0; i < len; i++)
		fprintf (stderr, "output[%lu] = U+%04x\n",
			 (unsigned long) i, q[i]);
	    }

	  q[len] = 0;
	  r = stringprep_ucs4_to_utf8 (q, -1, NULL, NULL);
	  free (q);
	  if (!r)
	    error (EXIT_FAILURE, 0,
		   _("could not convert from UCS-4 to UTF-8"));

	  p = stringprep_utf8_to_locale (r);
	  free (r);
	  if (!r)
	    error (EXIT_FAILURE, 0, _("could not convert from UTF-8 to %s"),
		   stringprep_locale_charset ());

	  fprintf (stdout, "%s\n", p);

	  free (p);
	}

      if (args_info.idna_to_ascii_given)
	{
	  p = stringprep_locale_to_utf8 (line);
	  if (!p)
	    error (EXIT_FAILURE, 0, _("could not convert from %s to UTF-8"),
		   stringprep_locale_charset ());

	  q = stringprep_utf8_to_ucs4 (p, -1, NULL);
	  free (p);
	  if (!q)
	    error (EXIT_FAILURE, 0,
		   _("could not convert from UCS-4 to UTF-8"));

	  if (args_info.debug_given)
	    {
	      size_t i;
	      for (i = 0; q[i]; i++)
		fprintf (stderr, "input[%lu] = U+%04x\n",
			 (unsigned long) i, q[i]);
	    }

	  rc = idna_to_ascii_4z (q, &p,
				 (args_info.allow_unassigned_given ?
				  IDNA_ALLOW_UNASSIGNED : 0) |
				 (args_info.usestd3asciirules_given ?
				  IDNA_USE_STD3_ASCII_RULES : 0));
	  free (q);
	  if (rc != IDNA_SUCCESS)
	    error (EXIT_FAILURE, 0, _("idna_to_ascii_4z: %s"),
		   idna_strerror (rc));

#ifdef WITH_TLD
	  if (args_info.tld_flag && !args_info.no_tld_flag)
	    {
	      size_t errpos;

	      rc = idna_to_unicode_8z4z (p, &q,
					 (args_info.allow_unassigned_given ?
					  IDNA_ALLOW_UNASSIGNED : 0) |
					 (args_info.usestd3asciirules_given ?
					  IDNA_USE_STD3_ASCII_RULES : 0));
	      if (rc != IDNA_SUCCESS)
		error (EXIT_FAILURE, 0, _("idna_to_unicode_8z4z (TLD): %s"),
		       idna_strerror (rc));

	      if (args_info.debug_given)
		{
		  size_t i;
		  for (i = 0; q[i]; i++)
		    fprintf (stderr, "tld[%lu] = U+%04x\n",
			     (unsigned long) i, q[i]);
		}

	      rc = tld_check_4z (q, &errpos, NULL);
	      free (q);
	      if (rc == TLD_INVALID)
		error (EXIT_FAILURE, 0, _("tld_check_4z (position %lu): %s"),
		       (unsigned long) errpos, tld_strerror (rc));
	      if (rc != TLD_SUCCESS)
		error (EXIT_FAILURE, 0, _("tld_check_4z: %s"),
		       tld_strerror (rc));
	    }
#endif

	  if (args_info.debug_given)
	    {
	      size_t i;
	      for (i = 0; p[i]; i++)
		fprintf (stderr, "output[%lu] = U+%04x\n",
			 (unsigned long) i, p[i]);
	    }

	  fprintf (stdout, "%s\n", p);

	  free (p);
	}

      if (args_info.idna_to_unicode_given)
	{
	  p = stringprep_locale_to_utf8 (line);
	  if (!p)
	    error (EXIT_FAILURE, 0, _("could not convert from %s to UTF-8"),
		   stringprep_locale_charset ());

	  q = stringprep_utf8_to_ucs4 (p, -1, NULL);
	  if (!q)
	    {
	      free (p);
	      error (EXIT_FAILURE, 0,
		     _("could not convert from UCS-4 to UTF-8"));
	    }

	  if (args_info.debug_given)
	    {
	      size_t i;
	      for (i = 0; q[i]; i++)
		fprintf (stderr, "input[%lu] = U+%04x\n",
			 (unsigned long) i, q[i]);
	    }
	  free (q);

	  rc = idna_to_unicode_8z4z (p, &q,
				     (args_info.allow_unassigned_given ?
				      IDNA_ALLOW_UNASSIGNED : 0) |
				     (args_info.usestd3asciirules_given ?
				      IDNA_USE_STD3_ASCII_RULES : 0));
	  free (p);
	  if (rc != IDNA_SUCCESS)
	    error (EXIT_FAILURE, 0, _("idna_to_unicode_8z4z: %s"),
		   idna_strerror (rc));

	  if (args_info.debug_given)
	    {
	      size_t i;
	      for (i = 0; q[i]; i++)
		fprintf (stderr, "output[%lu] = U+%04x\n",
			 (unsigned long) i, q[i]);
	    }

#ifdef WITH_TLD
	  if (args_info.tld_flag)
	    {
	      size_t errpos;

	      rc = tld_check_4z (q, &errpos, NULL);
	      if (rc == TLD_INVALID)
		{
		  free (q);
		  error (EXIT_FAILURE, 0,
			 _("tld_check_4z (position %lu): %s"),
			 (unsigned long) errpos, tld_strerror (rc));
		}
	      if (rc != TLD_SUCCESS)
		{
		  free (q);
		  error (EXIT_FAILURE, 0, _("tld_check_4z: %s"),
			 tld_strerror (rc));
		}
	    }
#endif

	  r = stringprep_ucs4_to_utf8 (q, -1, NULL, NULL);
	  free (q);
	  if (!r)
	    error (EXIT_FAILURE, 0,
		   _("could not convert from UTF-8 to UCS-4"));

	  p = stringprep_utf8_to_locale (r);
	  free (r);
	  if (!p)
	    error (EXIT_FAILURE, 0, _("could not convert from UTF-8 to %s"),
		   stringprep_locale_charset ());

	  fprintf (stdout, "%s\n", p);

	  free (p);
	}

      if (args_info.nfkc_given)
	{
	  p = stringprep_locale_to_utf8 (line);
	  if (!p)
	    error (EXIT_FAILURE, 0, _("could not convert from %s to UTF-8"),
		   stringprep_locale_charset ());

	  if (args_info.debug_given)
	    {
	      size_t i;

	      q = stringprep_utf8_to_ucs4 (p, -1, NULL);
	      if (!q)
		{
		  free (p);
		  error (EXIT_FAILURE, 0,
			 _("could not convert from UTF-8 to UCS-4"));
		}

	      for (i = 0; q[i]; i++)
		fprintf (stderr, "input[%lu] = U+%04x\n",
			 (unsigned long) i, q[i]);

	      free (q);
	    }

	  r = stringprep_utf8_nfkc_normalize (p, -1);
	  free (p);
	  if (!r)
	    error (EXIT_FAILURE, 0, _("could not do NFKC normalization"));

	  if (args_info.debug_given)
	    {
	      size_t i;

	      q = stringprep_utf8_to_ucs4 (r, -1, NULL);
	      if (!q)
		{
		  free (r);
		  error (EXIT_FAILURE, 0,
			 _("could not convert from UTF-8 to UCS-4"));
		}

	      for (i = 0; q[i]; i++)
		fprintf (stderr, "output[%lu] = U+%04x\n",
			 (unsigned long) i, q[i]);

	      free (q);
	    }

	  p = stringprep_utf8_to_locale (r);
	  free (r);
	  if (!p)
	    error (EXIT_FAILURE, 0, _("could not convert from UTF-8 to %s"),
		   stringprep_locale_charset ());

	  fprintf (stdout, "%s\n", p);

	  free (p);
	}

      fflush (stdout);
    }
  while (!feof (stdin) && !ferror (stdin) && (args_info.inputs_num == 0 ||
					      cmdn < args_info.inputs_num));

  free (line);

  return EXIT_SUCCESS;
}
