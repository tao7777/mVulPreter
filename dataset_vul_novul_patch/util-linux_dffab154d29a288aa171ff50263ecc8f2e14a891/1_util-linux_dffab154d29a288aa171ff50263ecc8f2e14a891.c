create_watching_parent (void)
{
  pid_t child;
  sigset_t ourset;
  struct sigaction oldact[3];
  int status = 0;
  int retval;

  retval = pam_open_session (pamh, 0);
  if (is_pam_failure(retval))
    {
      cleanup_pam (retval);
      errx (EXIT_FAILURE, _("cannot open session: %s"),
	     pam_strerror (pamh, retval));
    }
  else
    _pam_session_opened = 1;

  memset(oldact, 0, sizeof(oldact));

  child = fork ();
  if (child == (pid_t) -1)
    {
      cleanup_pam (PAM_ABORT);
      err (EXIT_FAILURE, _("cannot create child process"));
    }

  /* the child proceeds to run the shell */
  if (child == 0)
    return;

  /* In the parent watch the child.  */

  /* su without pam support does not have a helper that keeps
     sitting on any directory so let's go to /.  */
  if (chdir ("/") != 0)
    warn (_("cannot change directory to %s"), "/");

  sigfillset (&ourset);
  if (sigprocmask (SIG_BLOCK, &ourset, NULL))
    {
      warn (_("cannot block signals"));
      caught_signal = true;
    }
  if (!caught_signal)
    {
      struct sigaction action;
      action.sa_handler = su_catch_sig;
      sigemptyset (&action.sa_mask);
      action.sa_flags = 0;
      sigemptyset (&ourset);
    if (!same_session)
      {
        if (sigaddset(&ourset, SIGINT) || sigaddset(&ourset, SIGQUIT))
          {
            warn (_("cannot set signal handler"));
            caught_signal = true;
          }
      }
    if (!caught_signal && (sigaddset(&ourset, SIGTERM)
                    || sigaddset(&ourset, SIGALRM)
                    || sigaction(SIGTERM, &action, &oldact[0])
                    || sigprocmask(SIG_UNBLOCK, &ourset, NULL))) {
	  warn (_("cannot set signal handler"));
	  caught_signal = true;
	}
    if (!caught_signal && !same_session && (sigaction(SIGINT, &action, &oldact[1])
                                     || sigaction(SIGQUIT, &action, &oldact[2])))
      {
        warn (_("cannot set signal handler"));
        caught_signal = true;
      }
    }
  if (!caught_signal)
    {
      pid_t pid;
      for (;;)
	{
	  pid = waitpid (child, &status, WUNTRACED);

	  if (pid != (pid_t)-1 && WIFSTOPPED (status))
	    {
	      kill (getpid (), SIGSTOP);
	      /* once we get here, we must have resumed */
	      kill (pid, SIGCONT);
	    }
	  else
	    break;
	}
      if (pid != (pid_t)-1)
        {
          if (WIFSIGNALED (status))
            {
              fprintf (stderr, "%s%s\n", strsignal (WTERMSIG (status)),
                       WCOREDUMP (status) ? _(" (core dumped)") : "");
              status = WTERMSIG (status) + 128;
             }
           else
             status = WEXITSTATUS (status);
         }
       else if (caught_signal)
         status = caught_signal + 128;
      else
        status = 1;
    }
   else
     status = 1;
 
  if (caught_signal)
     {
       fprintf (stderr, _("\nSession terminated, killing shell..."));
       kill (child, SIGTERM);
    }

  cleanup_pam (PAM_SUCCESS);
 
   if (caught_signal)
     {
      sleep (2);
      kill (child, SIGKILL);
      fprintf (stderr, _(" ...killed.\n"));
 
       /* Let's terminate itself with the received signal.
        *
       * It seems that shells use WIFSIGNALED() rather than our exit status
       * value to detect situations when is necessary to cleanup (reset)
       * terminal settings (kzak -- Jun 2013).
       */
      switch (caught_signal) {
        case SIGTERM:
          sigaction(SIGTERM, &oldact[0], NULL);
          break;
        case SIGINT:
          sigaction(SIGINT, &oldact[1], NULL);
          break;
        case SIGQUIT:
          sigaction(SIGQUIT, &oldact[2], NULL);
          break;
        default:
	  /* just in case that signal stuff initialization failed and
	   * caught_signal = true */
          caught_signal = SIGKILL;
          break;
      }
      kill(getpid(), caught_signal);
    }
  exit (status);
}
