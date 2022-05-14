getftp (struct url *u, wgint passed_expected_bytes, wgint *qtyread,
        wgint restval, ccon *con, int count, wgint *last_expected_bytes,
        FILE *warc_tmp)
{
  int csock, dtsock, local_sock, res;
  uerr_t err = RETROK;          /* appease the compiler */
  FILE *fp;
   char *respline, *tms;
   const char *user, *passwd, *tmrate;
   int cmd = con->cmd;
   wgint expected_bytes = 0;
   bool got_expected_bytes = false;
   bool rest_failed = false;
  bool rest_failed = false;
  int flags;
  wgint rd_size, previous_rd_size = 0;
  char type_char;
  bool try_again;
  bool list_a_used = false;

  assert (con != NULL);
  assert (con->target != NULL);

  /* Debug-check of the sanity of the request by making sure that LIST
     and RETR are never both requested (since we can handle only one
     at a time.  */
  assert (!((cmd & DO_LIST) && (cmd & DO_RETR)));
  /* Make sure that at least *something* is requested.  */
  assert ((cmd & (DO_LIST | DO_CWD | DO_RETR | DO_LOGIN)) != 0);

  *qtyread = restval;

  user = u->user;
  passwd = u->passwd;
  search_netrc (u->host, (const char **)&user, (const char **)&passwd, 1);
  user = user ? user : (opt.ftp_user ? opt.ftp_user : opt.user);
  if (!user) user = "anonymous";
  passwd = passwd ? passwd : (opt.ftp_passwd ? opt.ftp_passwd : opt.passwd);
  if (!passwd) passwd = "-wget@";

  dtsock = -1;
  local_sock = -1;
  con->dltime = 0;

  if (!(cmd & DO_LOGIN))
    csock = con->csock;
  else                          /* cmd & DO_LOGIN */
    {
      char    *host = con->proxy ? con->proxy->host : u->host;
      int      port = con->proxy ? con->proxy->port : u->port;

      /* Login to the server: */

      /* First: Establish the control connection.  */

      csock = connect_to_host (host, port);
      if (csock == E_HOST)
          return HOSTERR;
      else if (csock < 0)
          return (retryable_socket_connect_error (errno)
                  ? CONERROR : CONIMPOSSIBLE);

      if (cmd & LEAVE_PENDING)
        con->csock = csock;
      else
        con->csock = -1;

      /* Second: Login with proper USER/PASS sequence.  */
      logprintf (LOG_VERBOSE, _("Logging in as %s ... "),
                 quotearg_style (escape_quoting_style, user));
      if (opt.server_response)
        logputs (LOG_ALWAYS, "\n");
      if (con->proxy)
        {
          /* If proxy is in use, log in as username@target-site. */
          char *logname = concat_strings (user, "@", u->host, (char *) 0);
          err = ftp_login (csock, logname, passwd);
          xfree (logname);
        }
      else
        err = ftp_login (csock, user, passwd);

      /* FTPRERR, FTPSRVERR, WRITEFAILED, FTPLOGREFUSED, FTPLOGINC */
      switch (err)
        {
        case FTPRERR:
          logputs (LOG_VERBOSE, "\n");
          logputs (LOG_NOTQUIET, _("\
Error in server response, closing control connection.\n"));
          fd_close (csock);
          con->csock = -1;
          return err;
        case FTPSRVERR:
          logputs (LOG_VERBOSE, "\n");
          logputs (LOG_NOTQUIET, _("Error in server greeting.\n"));
          fd_close (csock);
          con->csock = -1;
          return err;
        case WRITEFAILED:
          logputs (LOG_VERBOSE, "\n");
          logputs (LOG_NOTQUIET,
                   _("Write failed, closing control connection.\n"));
          fd_close (csock);
          con->csock = -1;
          return err;
        case FTPLOGREFUSED:
          logputs (LOG_VERBOSE, "\n");
          logputs (LOG_NOTQUIET, _("The server refuses login.\n"));
          fd_close (csock);
          con->csock = -1;
          return FTPLOGREFUSED;
        case FTPLOGINC:
          logputs (LOG_VERBOSE, "\n");
          logputs (LOG_NOTQUIET, _("Login incorrect.\n"));
          fd_close (csock);
          con->csock = -1;
          return FTPLOGINC;
        case FTPOK:
          if (!opt.server_response)
            logputs (LOG_VERBOSE, _("Logged in!\n"));
          break;
        default:
          abort ();
        }
      /* Third: Get the system type */
      if (!opt.server_response)
        logprintf (LOG_VERBOSE, "==> SYST ... ");
      err = ftp_syst (csock, &con->rs, &con->rsu);
      /* FTPRERR */
      switch (err)
        {
        case FTPRERR:
          logputs (LOG_VERBOSE, "\n");
          logputs (LOG_NOTQUIET, _("\
Error in server response, closing control connection.\n"));
          fd_close (csock);
          con->csock = -1;
          return err;
        case FTPSRVERR:
          logputs (LOG_VERBOSE, "\n");
          logputs (LOG_NOTQUIET,
                   _("Server error, can't determine system type.\n"));
          break;
        case FTPOK:
          /* Everything is OK.  */
          break;
        default:
          abort ();
        }
      if (!opt.server_response && err != FTPSRVERR)
        logputs (LOG_VERBOSE, _("done.    "));

      /* 2013-10-17 Andrea Urbani (matfanjol)
         According to the system type I choose which
         list command will be used.
         If I don't know that system, I will try, the
         first time of each session, "LIST -a" and
         "LIST". (see __LIST_A_EXPLANATION__ below) */
      switch (con->rs)
        {
        case ST_VMS:
          /* About ST_VMS there is an old note:
             2008-01-29  SMS.  For a VMS FTP server, where "LIST -a" may not
             fail, but will never do what is desired here,
             skip directly to the simple "LIST" command
             (assumed to be the last one in the list).  */
          DEBUGP (("\nVMS: I know it and I will use \"LIST\" as standard list command\n"));
          con->st |= LIST_AFTER_LIST_A_CHECK_DONE;
          con->st |= AVOID_LIST_A;
          break;
        case ST_UNIX:
          if (con->rsu == UST_MULTINET)
            {
              DEBUGP (("\nUNIX MultiNet: I know it and I will use \"LIST\" "
                       "as standard list command\n"));
              con->st |= LIST_AFTER_LIST_A_CHECK_DONE;
              con->st |= AVOID_LIST_A;
            }
          else if (con->rsu == UST_TYPE_L8)
            {
              DEBUGP (("\nUNIX TYPE L8: I know it and I will use \"LIST -a\" "
                       "as standard list command\n"));
              con->st |= LIST_AFTER_LIST_A_CHECK_DONE;
              con->st |= AVOID_LIST;
            }
          break;
        default:
          break;
        }

      /* Fourth: Find the initial ftp directory */

      if (!opt.server_response)
        logprintf (LOG_VERBOSE, "==> PWD ... ");
      err = ftp_pwd (csock, &con->id);
      /* FTPRERR */
      switch (err)
        {
        case FTPRERR:
          logputs (LOG_VERBOSE, "\n");
          logputs (LOG_NOTQUIET, _("\
Error in server response, closing control connection.\n"));
          fd_close (csock);
          con->csock = -1;
          return err;
        case FTPSRVERR :
          /* PWD unsupported -- assume "/". */
          xfree (con->id);
          con->id = xstrdup ("/");
          break;
        case FTPOK:
          /* Everything is OK.  */
          break;
        default:
          abort ();
        }

#if 0
      /* 2004-09-17 SMS.
         Don't help me out.  Please.
         A reasonably recent VMS FTP server will cope just fine with
         UNIX file specifications.  This code just spoils things.
         Discarding the device name, for example, is not a wise move.
         This code was disabled but left in as an example of what not
         to do.
      */

      /* VMS will report something like "PUB$DEVICE:[INITIAL.FOLDER]".
         Convert it to "/INITIAL/FOLDER" */
      if (con->rs == ST_VMS)
        {
          char *path = strchr (con->id, '[');
          char *pathend = path ? strchr (path + 1, ']') : NULL;
          if (!path || !pathend)
            DEBUGP (("Initial VMS directory not in the form [...]!\n"));
          else
            {
              char *idir = con->id;
              DEBUGP (("Preprocessing the initial VMS directory\n"));
              DEBUGP (("  old = '%s'\n", con->id));
              /* We do the conversion in-place by copying the stuff
                 between [ and ] to the beginning, and changing dots
                 to slashes at the same time.  */
              *idir++ = '/';
              for (++path; path < pathend; path++, idir++)
                *idir = *path == '.' ? '/' : *path;
              *idir = '\0';
              DEBUGP (("  new = '%s'\n\n", con->id));
            }
        }
#endif /* 0 */

      if (!opt.server_response)
        logputs (LOG_VERBOSE, _("done.\n"));

      /* Fifth: Set the FTP type.  */
      type_char = ftp_process_type (u->params);
      if (!opt.server_response)
        logprintf (LOG_VERBOSE, "==> TYPE %c ... ", type_char);
      err = ftp_type (csock, type_char);
      /* FTPRERR, WRITEFAILED, FTPUNKNOWNTYPE */
      switch (err)
        {
        case FTPRERR:
          logputs (LOG_VERBOSE, "\n");
          logputs (LOG_NOTQUIET, _("\
Error in server response, closing control connection.\n"));
          fd_close (csock);
          con->csock = -1;
          return err;
        case WRITEFAILED:
          logputs (LOG_VERBOSE, "\n");
          logputs (LOG_NOTQUIET,
                   _("Write failed, closing control connection.\n"));
          fd_close (csock);
          con->csock = -1;
          return err;
        case FTPUNKNOWNTYPE:
          logputs (LOG_VERBOSE, "\n");
          logprintf (LOG_NOTQUIET,
                     _("Unknown type `%c', closing control connection.\n"),
                     type_char);
          fd_close (csock);
          con->csock = -1;
          return err;
        case FTPOK:
          /* Everything is OK.  */
          break;
        default:
          abort ();
        }
      if (!opt.server_response)
        logputs (LOG_VERBOSE, _("done.  "));
    } /* do login */

  if (cmd & DO_CWD)
    {
      if (!*u->dir)
        logputs (LOG_VERBOSE, _("==> CWD not needed.\n"));
      else
        {
          const char *targ = NULL;
          int cwd_count;
          int cwd_end;
          int cwd_start;

          char *target = u->dir;

          DEBUGP (("changing working directory\n"));

          /* Change working directory.  To change to a non-absolute
             Unix directory, we need to prepend initial directory
             (con->id) to it.  Absolute directories "just work".

             A relative directory is one that does not begin with '/'
             and, on non-Unix OS'es, one that doesn't begin with
             "[a-z]:".

             This is not done for OS400, which doesn't use
             "/"-delimited directories, nor does it support directory
             hierarchies.  "CWD foo" followed by "CWD bar" leaves us
             in "bar", not in "foo/bar", as would be customary
             elsewhere.  */

            /* 2004-09-20 SMS.
               Why is this wise even on UNIX?  It certainly fouls VMS.
               See below for a more reliable, more universal method.
            */

            /* 2008-04-22 MJC.
               I'm not crazy about it either. I'm informed it's useful
               for misconfigured servers that have some dirs in the path
               with +x but -r, but this method is not RFC-conformant. I
               understand the need to deal with crappy server
               configurations, but it's far better to use the canonical
               method first, and fall back to kludges second.
            */

          if (target[0] != '/'
              && !(con->rs != ST_UNIX
                   && c_isalpha (target[0])
                   && target[1] == ':')
              && (con->rs != ST_OS400)
              && (con->rs != ST_VMS))
            {
              int idlen = strlen (con->id);
              char *ntarget, *p;

              /* Strip trailing slash(es) from con->id. */
              while (idlen > 0 && con->id[idlen - 1] == '/')
                --idlen;
              p = ntarget = (char *)alloca (idlen + 1 + strlen (u->dir) + 1);
              memcpy (p, con->id, idlen);
              p += idlen;
              *p++ = '/';
              strcpy (p, target);

              DEBUGP (("Prepended initial PWD to relative path:\n"));
              DEBUGP (("   pwd: '%s'\n   old: '%s'\n  new: '%s'\n",
                       con->id, target, ntarget));
              target = ntarget;
            }

#if 0
          /* 2004-09-17 SMS.
             Don't help me out.  Please.
             A reasonably recent VMS FTP server will cope just fine with
             UNIX file specifications.  This code just spoils things.
             Discarding the device name, for example, is not a wise
             move.
             This code was disabled but left in as an example of what
             not to do.
          */

          /* If the FTP host runs VMS, we will have to convert the absolute
             directory path in UNIX notation to absolute directory path in
             VMS notation as VMS FTP servers do not like UNIX notation of
             absolute paths.  "VMS notation" is [dir.subdir.subsubdir]. */

          if (con->rs == ST_VMS)
            {
              char *tmpp;
              char *ntarget = (char *)alloca (strlen (target) + 2);
              /* We use a converted initial dir, so directories in
                 TARGET will be separated with slashes, something like
                 "/INITIAL/FOLDER/DIR/SUBDIR".  Convert that to
                 "[INITIAL.FOLDER.DIR.SUBDIR]".  */
              strcpy (ntarget, target);
              assert (*ntarget == '/');
              *ntarget = '[';
              for (tmpp = ntarget + 1; *tmpp; tmpp++)
                if (*tmpp == '/')
                  *tmpp = '.';
              *tmpp++ = ']';
              *tmpp = '\0';
              DEBUGP (("Changed file name to VMS syntax:\n"));
              DEBUGP (("  Unix: '%s'\n  VMS: '%s'\n", target, ntarget));
              target = ntarget;
            }
#endif /* 0 */

          /* 2004-09-20 SMS.
             A relative directory is relative to the initial directory.
             Thus, what _is_ useful on VMS (and probably elsewhere) is
             to CWD to the initial directory (ideally, whatever the
             server reports, _exactly_, NOT badly UNIX-ixed), and then
             CWD to the (new) relative directory.  This should probably
             be restructured as a function, called once or twice, but
             I'm lazy enough to take the badly indented loop short-cut
             for now.
          */

          /* Decide on one pass (absolute) or two (relative).
             The VMS restriction may be relaxed when the squirrely code
             above is reformed.
          */
          if ((con->rs == ST_VMS) && (target[0] != '/'))
            {
              cwd_start = 0;
              DEBUGP (("Using two-step CWD for relative path.\n"));
            }
          else
            {
              /* Go straight to the target. */
              cwd_start = 1;
            }

          /* At least one VMS FTP server (TCPware V5.6-2) can switch to
             a UNIX emulation mode when given a UNIX-like directory
             specification (like "a/b/c").  If allowed to continue this
             way, LIST interpretation will be confused, because the
             system type (SYST response) will not be re-checked, and
             future UNIX-format directory listings (for multiple URLs or
             "-r") will be horribly misinterpreted.

             The cheap and nasty work-around is to do a "CWD []" after a
             UNIX-like directory specification is used.  (A single-level
             directory is harmless.)  This puts the TCPware server back
             into VMS mode, and does no harm on other servers.

             Unlike the rest of this block, this particular behavior
             _is_ VMS-specific, so it gets its own VMS test.
          */
          if ((con->rs == ST_VMS) && (strchr( target, '/') != NULL))
            {
              cwd_end = 3;
              DEBUGP (("Using extra \"CWD []\" step for VMS server.\n"));
            }
          else
            {
              cwd_end = 2;
            }

          /* 2004-09-20 SMS. */
          /* Sorry about the deviant indenting.  Laziness. */

          for (cwd_count = cwd_start; cwd_count < cwd_end; cwd_count++)
            {
              switch (cwd_count)
                {
                  case 0:
                    /* Step one (optional): Go to the initial directory,
                       exactly as reported by the server.
                    */
                    targ = con->id;
                    break;

                  case 1:
                    /* Step two: Go to the target directory.  (Absolute or
                       relative will work now.)
                    */
                    targ = target;
                    break;

                  case 2:
                    /* Step three (optional): "CWD []" to restore server
                       VMS-ness.
                    */
                    targ = "[]";
                    break;

                  default:
                    logprintf (LOG_ALWAYS, _("Logically impossible section reached in getftp()"));
                    logprintf (LOG_ALWAYS, _("cwd_count: %d\ncwd_start: %d\ncwd_end: %d\n"),
                                             cwd_count, cwd_start, cwd_end);
                    abort ();
                }

              if (!opt.server_response)
                logprintf (LOG_VERBOSE, "==> CWD (%d) %s ... ", cwd_count,
                           quotearg_style (escape_quoting_style, target));

              err = ftp_cwd (csock, targ);

              /* FTPRERR, WRITEFAILED, FTPNSFOD */
              switch (err)
                {
                  case FTPRERR:
                    logputs (LOG_VERBOSE, "\n");
                    logputs (LOG_NOTQUIET, _("\
Error in server response, closing control connection.\n"));
                    fd_close (csock);
                    con->csock = -1;
                    return err;
                  case WRITEFAILED:
                    logputs (LOG_VERBOSE, "\n");
                    logputs (LOG_NOTQUIET,
                             _("Write failed, closing control connection.\n"));
                    fd_close (csock);
                    con->csock = -1;
                    return err;
                  case FTPNSFOD:
                    logputs (LOG_VERBOSE, "\n");
                    logprintf (LOG_NOTQUIET, _("No such directory %s.\n\n"),
                               quote (u->dir));
                    fd_close (csock);
                    con->csock = -1;
                    return err;
                  case FTPOK:
                    break;
                  default:
                    abort ();
                }

              if (!opt.server_response)
                logputs (LOG_VERBOSE, _("done.\n"));

            } /* for */

          /* 2004-09-20 SMS. */

        } /* else */
    }
  else /* do not CWD */
    logputs (LOG_VERBOSE, _("==> CWD not required.\n"));

  if ((cmd & DO_RETR) && passed_expected_bytes == 0)
    {
      if (opt.verbose)
        {
          if (!opt.server_response)
            logprintf (LOG_VERBOSE, "==> SIZE %s ... ",
                       quotearg_style (escape_quoting_style, u->file));
        }

      err = ftp_size (csock, u->file, &expected_bytes);
      /* FTPRERR */
      switch (err)
        {
        case FTPRERR:
        case FTPSRVERR:
          logputs (LOG_VERBOSE, "\n");
          logputs (LOG_NOTQUIET, _("\
Error in server response, closing control connection.\n"));
          fd_close (csock);
          con->csock = -1;
          return err;
        case FTPOK:
          got_expected_bytes = true;
          /* Everything is OK.  */
          break;
        default:
          abort ();
        }
        if (!opt.server_response)
          {
            logprintf (LOG_VERBOSE, "%s\n",
                    expected_bytes ?
                    number_to_static_string (expected_bytes) :
                    _("done.\n"));
          }
    }

  if (cmd & DO_RETR && restval > 0 && restval == expected_bytes)
    {
      /* Server confirms that file has length restval. We should stop now.
         Some servers (f.e. NcFTPd) return error when receive REST 0 */
      logputs (LOG_VERBOSE, _("File has already been retrieved.\n"));
      fd_close (csock);
      con->csock = -1;
      return RETRFINISHED;
    }

  do
  {
  try_again = false;
  /* If anything is to be retrieved, PORT (or PASV) must be sent.  */
  if (cmd & (DO_LIST | DO_RETR))
    {
      if (opt.ftp_pasv)
        {
          ip_address passive_addr;
          int        passive_port;
          err = ftp_do_pasv (csock, &passive_addr, &passive_port);
          /* FTPRERR, WRITEFAILED, FTPNOPASV, FTPINVPASV */
          switch (err)
            {
            case FTPRERR:
              logputs (LOG_VERBOSE, "\n");
              logputs (LOG_NOTQUIET, _("\
Error in server response, closing control connection.\n"));
              fd_close (csock);
              con->csock = -1;
              return err;
            case WRITEFAILED:
              logputs (LOG_VERBOSE, "\n");
              logputs (LOG_NOTQUIET,
                       _("Write failed, closing control connection.\n"));
              fd_close (csock);
              con->csock = -1;
              return err;
            case FTPNOPASV:
              logputs (LOG_VERBOSE, "\n");
              logputs (LOG_NOTQUIET, _("Cannot initiate PASV transfer.\n"));
              break;
            case FTPINVPASV:
              logputs (LOG_VERBOSE, "\n");
              logputs (LOG_NOTQUIET, _("Cannot parse PASV response.\n"));
              break;
            case FTPOK:
              break;
            default:
              abort ();
            }   /* switch (err) */
          if (err==FTPOK)
            {
              DEBUGP (("trying to connect to %s port %d\n",
                      print_address (&passive_addr), passive_port));
              dtsock = connect_to_ip (&passive_addr, passive_port, NULL);
              if (dtsock < 0)
                {
                  int save_errno = errno;
                  fd_close (csock);
                  con->csock = -1;
                  logprintf (LOG_VERBOSE, _("couldn't connect to %s port %d: %s\n"),
                             print_address (&passive_addr), passive_port,
                             strerror (save_errno));
                           ? CONERROR : CONIMPOSSIBLE);
                 }
 
               if (!opt.server_response)
                 logputs (LOG_VERBOSE, _("done.    "));
            }
          else
            return err;
 
          /*
           * We do not want to fall back from PASSIVE mode to ACTIVE mode !
           * The reason is the PORT command exposes the client's real IP address
           * to the server. Bad for someone who relies on privacy via a ftp proxy.
           */
        }
      else
         {
           err = ftp_do_port (csock, &local_sock);
           /* FTPRERR, WRITEFAILED, bindport (FTPSYSERR), HOSTERR,
              logputs (LOG_NOTQUIET, _("\
Error in server response, closing control connection.\n"));
              fd_close (csock);
              con->csock = -1;
              fd_close (dtsock);
              fd_close (local_sock);
              return err;
            case WRITEFAILED:
              logputs (LOG_VERBOSE, "\n");
              logputs (LOG_NOTQUIET,
                       _("Write failed, closing control connection.\n"));
              fd_close (csock);
              con->csock = -1;
              fd_close (dtsock);
              fd_close (local_sock);
              return err;
            case CONSOCKERR:
              logputs (LOG_VERBOSE, "\n");
              logprintf (LOG_NOTQUIET, "socket: %s\n", strerror (errno));
              fd_close (csock);
              con->csock = -1;
              fd_close (dtsock);
              fd_close (local_sock);
              return err;
            case FTPSYSERR:
              logputs (LOG_VERBOSE, "\n");
              logprintf (LOG_NOTQUIET, _("Bind error (%s).\n"),
                         strerror (errno));
              fd_close (dtsock);
              return err;
            case FTPPORTERR:
              logputs (LOG_VERBOSE, "\n");
              logputs (LOG_NOTQUIET, _("Invalid PORT.\n"));
              fd_close (csock);
              con->csock = -1;
              fd_close (dtsock);
              fd_close (local_sock);
              return err;
            case FTPOK:
              break;
            default:
              abort ();
            } /* port switch */
          if (!opt.server_response)
            logputs (LOG_VERBOSE, _("done.    "));
        } /* dtsock == -1 */
    } /* cmd & (DO_LIST | DO_RETR) */

  /* Restart if needed.  */
  if (restval && (cmd & DO_RETR))
    {
      if (!opt.server_response)
        logprintf (LOG_VERBOSE, "==> REST %s ... ",
                   number_to_static_string (restval));
      err = ftp_rest (csock, restval);

      /* FTPRERR, WRITEFAILED, FTPRESTFAIL */
      switch (err)
        {
        case FTPRERR:
          logputs (LOG_VERBOSE, "\n");
          logputs (LOG_NOTQUIET, _("\
Error in server response, closing control connection.\n"));
          fd_close (csock);
          con->csock = -1;
          fd_close (dtsock);
          fd_close (local_sock);
          return err;
        case WRITEFAILED:
          logputs (LOG_VERBOSE, "\n");
          logputs (LOG_NOTQUIET,
                   _("Write failed, closing control connection.\n"));
          fd_close (csock);
          con->csock = -1;
          fd_close (dtsock);
          fd_close (local_sock);
          return err;
        case FTPRESTFAIL:
          logputs (LOG_VERBOSE, _("\nREST failed, starting from scratch.\n"));
          rest_failed = true;
          break;
        case FTPOK:
          break;
        default:
          abort ();
        }
      if (err != FTPRESTFAIL && !opt.server_response)
        logputs (LOG_VERBOSE, _("done.    "));
    } /* restval && cmd & DO_RETR */

  if (cmd & DO_RETR)
    {
      /* If we're in spider mode, don't really retrieve anything except
         the directory listing and verify whether the given "file" exists.  */
      if (opt.spider)
        {
          bool exists = false;
          struct fileinfo *f;
          uerr_t _res = ftp_get_listing (u, con, &f);
          /* Set the DO_RETR command flag again, because it gets unset when
             calling ftp_get_listing() and would otherwise cause an assertion
             failure earlier on when this function gets repeatedly called
             (e.g., when recursing).  */
          con->cmd |= DO_RETR;
          if (_res == RETROK)
            {
              while (f)
                {
                  if (!strcmp (f->name, u->file))
                    {
                      exists = true;
                      break;
                    }
                  f = f->next;
                }
              if (exists)
                {
                  logputs (LOG_VERBOSE, "\n");
                  logprintf (LOG_NOTQUIET, _("File %s exists.\n"),
                             quote (u->file));
                }
              else
                {
                  logputs (LOG_VERBOSE, "\n");
                  logprintf (LOG_NOTQUIET, _("No such file %s.\n"),
                             quote (u->file));
                }
            }
          fd_close (csock);
          con->csock = -1;
          fd_close (dtsock);
          fd_close (local_sock);
          return RETRFINISHED;
        }

      if (opt.verbose)
        {
          if (!opt.server_response)
            {
              if (restval)
                logputs (LOG_VERBOSE, "\n");
              logprintf (LOG_VERBOSE, "==> RETR %s ... ",
                         quotearg_style (escape_quoting_style, u->file));
            }
        }

      err = ftp_retr (csock, u->file);
      /* FTPRERR, WRITEFAILED, FTPNSFOD */
      switch (err)
        {
        case FTPRERR:
          logputs (LOG_VERBOSE, "\n");
          logputs (LOG_NOTQUIET, _("\
Error in server response, closing control connection.\n"));
          fd_close (csock);
          con->csock = -1;
          fd_close (dtsock);
          fd_close (local_sock);
          return err;
        case WRITEFAILED:
          logputs (LOG_VERBOSE, "\n");
          logputs (LOG_NOTQUIET,
                   _("Write failed, closing control connection.\n"));
          fd_close (csock);
          con->csock = -1;
          fd_close (dtsock);
          fd_close (local_sock);
          return err;
        case FTPNSFOD:
          logputs (LOG_VERBOSE, "\n");
          logprintf (LOG_NOTQUIET, _("No such file %s.\n\n"),
                     quote (u->file));
          fd_close (dtsock);
          fd_close (local_sock);
          return err;
        case FTPOK:
          break;
        default:
          abort ();
        }

      if (!opt.server_response)
        logputs (LOG_VERBOSE, _("done.\n"));

      if (! got_expected_bytes)
        expected_bytes = *last_expected_bytes;
    } /* do retrieve */

  if (cmd & DO_LIST)
    {
      if (!opt.server_response)
        logputs (LOG_VERBOSE, "==> LIST ... ");
      /* As Maciej W. Rozycki (macro@ds2.pg.gda.pl) says, `LIST'
         without arguments is better than `LIST .'; confirmed by
         RFC959.  */
      err = ftp_list (csock, NULL, con->st&AVOID_LIST_A, con->st&AVOID_LIST, &list_a_used);

      /* FTPRERR, WRITEFAILED */
      switch (err)
        {
        case FTPRERR:
          logputs (LOG_VERBOSE, "\n");
          logputs (LOG_NOTQUIET, _("\
Error in server response, closing control connection.\n"));
          fd_close (csock);
          con->csock = -1;
          fd_close (dtsock);
          fd_close (local_sock);
          return err;
        case WRITEFAILED:
          logputs (LOG_VERBOSE, "\n");
          logputs (LOG_NOTQUIET,
                   _("Write failed, closing control connection.\n"));
          fd_close (csock);
          con->csock = -1;
          fd_close (dtsock);
          fd_close (local_sock);
          return err;
        case FTPNSFOD:
          logputs (LOG_VERBOSE, "\n");
          logprintf (LOG_NOTQUIET, _("No such file or directory %s.\n\n"),
                     quote ("."));
          fd_close (dtsock);
          fd_close (local_sock);
          return err;
        case FTPOK:
          break;
        default:
          abort ();
        }
      if (!opt.server_response)
        logputs (LOG_VERBOSE, _("done.\n"));

      if (! got_expected_bytes)
        expected_bytes = *last_expected_bytes;
    } /* cmd & DO_LIST */

  if (!(cmd & (DO_LIST | DO_RETR)) || (opt.spider && !(cmd & DO_LIST)))
    return RETRFINISHED;

  /* Some FTP servers return the total length of file after REST
     command, others just return the remaining size. */
  if (passed_expected_bytes && restval && expected_bytes
      && (expected_bytes == passed_expected_bytes - restval))
    {
      DEBUGP (("Lying FTP server found, adjusting.\n"));
      expected_bytes = passed_expected_bytes;
    }

  /* If no transmission was required, then everything is OK.  */
  if (!pasv_mode_open)  /* we are not using pasive mode so we need
                              to accept */
     }
