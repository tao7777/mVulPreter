check_user_token (const char *authfile,
		  const char *username,
		  const char *otp_id,
		  int verbose,
                  FILE *debug_file)
{
  char buf[1024];
  char *s_user, *s_token;
  int retval = AUTH_ERROR;
  int fd;
  struct stat st;
  FILE *opwfile;

  fd = open(authfile, O_RDONLY, 0);
  if (fd < 0) {
      if(verbose)
	  D (debug_file, "Cannot open file: %s (%s)", authfile, strerror(errno));
      return retval;
  }

  if (fstat(fd, &st) < 0) {
      if(verbose)
	  D (debug_file, "Cannot stat file: %s (%s)", authfile, strerror(errno));
      close(fd);
      return retval;
  }

  if (!S_ISREG(st.st_mode)) {
      if(verbose)
	  D (debug_file, "%s is not a regular file", authfile);
      close(fd);
      return retval;
  }

  opwfile = fdopen(fd, "r");
  if (opwfile == NULL) {
      if(verbose)
	  D (debug_file, "fdopen: %s", strerror(errno));
      close(fd);
      return retval;
  }

  retval = AUTH_NO_TOKENS;
  while (fgets (buf, 1024, opwfile))
    {
      char *saveptr = NULL;
      if (buf[strlen (buf) - 1] == '\n')
	buf[strlen (buf) - 1] = '\0';
      if (buf[0] == '#') {
          /* This is a comment and we may skip it. */
          if(verbose)
              D (debug_file, "Skipping comment line: %s", buf);
          continue;
      }
      if(verbose)
	  D (debug_file, "Authorization line: %s", buf);
      s_user = strtok_r (buf, ":", &saveptr);
      if (s_user && strcmp (username, s_user) == 0)
	{
	  if(verbose)
	      D (debug_file, "Matched user: %s", s_user);
      retval = AUTH_NOT_FOUND; /* We found at least one line for the user */
	  do
	    {
	      s_token = strtok_r (NULL, ":", &saveptr);
	      if(verbose)
		  D (debug_file, "Authorization token: %s", s_token);
	      if (s_token && otp_id && strcmp (otp_id, s_token) == 0)
 		{
 		  if(verbose)
 		      D (debug_file, "Match user/token as %s/%s", username, otp_id);

		  fclose(opwfile);
 		  return AUTH_FOUND;
 		}
 	    }
	  while (s_token != NULL);
	}
    }

  fclose (opwfile);

  return retval;
}
