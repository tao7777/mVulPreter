LockServer(void)
{
  char tmp[PATH_MAX], pid_str[12];
  int lfd, i, haslock, l_pid, t;
  char *tmppath = NULL;
  int len;
  char port[20];

  if (nolock) return;
  /*
   * Path names
   */
  tmppath = LOCK_DIR;

  sprintf(port, "%d", atoi(display));
  len = strlen(LOCK_PREFIX) > strlen(LOCK_TMP_PREFIX) ? strlen(LOCK_PREFIX) :
						strlen(LOCK_TMP_PREFIX);
  len += strlen(tmppath) + strlen(port) + strlen(LOCK_SUFFIX) + 1;
  if (len > sizeof(LockFile))
    FatalError("Display name `%s' is too long\n", port);
  (void)sprintf(tmp, "%s" LOCK_TMP_PREFIX "%s" LOCK_SUFFIX, tmppath, port);
  (void)sprintf(LockFile, "%s" LOCK_PREFIX "%s" LOCK_SUFFIX, tmppath, port);

  /*
   * Create a temporary file containing our PID.  Attempt three times
   * to create the file.
   */
  StillLocking = TRUE;
  i = 0;
  do {
    i++;
    lfd = open(tmp, O_CREAT | O_EXCL | O_WRONLY, 0644);
    if (lfd < 0)
       sleep(2);
    else
       break;
  } while (i < 3);
  if (lfd < 0) {
    unlink(tmp);
    i = 0;
    do {
      i++;
      lfd = open(tmp, O_CREAT | O_EXCL | O_WRONLY, 0644);
      if (lfd < 0)
         sleep(2);
      else
         break;
    } while (i < 3);
  }
  if (lfd < 0)
     FatalError("Could not create lock file in %s\n", tmp);
   (void) sprintf(pid_str, "%10ld\n", (long)getpid());
   (void) write(lfd, pid_str, 11);
  (void) fchmod(lfd, 0444);
   (void) close(lfd);
 
   /*
   * OK.  Now the tmp file exists.  Try three times to move it in place
   * for the lock.
   */
  i = 0;
  haslock = 0;
  while ((!haslock) && (i++ < 3)) {
    haslock = (link(tmp,LockFile) == 0);
    if (haslock) {
      /*
       * We're done.
       */
      break;
    }
    else {
      /*
       * Read the pid from the existing file
       */
      lfd = open(LockFile, O_RDONLY|O_NOFOLLOW);
      if (lfd < 0) {
        unlink(tmp);
        FatalError("Can't read lock file %s\n", LockFile);
      }
      pid_str[0] = '\0';
      if (read(lfd, pid_str, 11) != 11) {
        /*
         * Bogus lock file.
         */
        unlink(LockFile);
        close(lfd);
        continue;
      }
      pid_str[11] = '\0';
      sscanf(pid_str, "%d", &l_pid);
      close(lfd);

      /*
       * Now try to kill the PID to see if it exists.
       */
      errno = 0;
      t = kill(l_pid, 0);
      if ((t< 0) && (errno == ESRCH)) {
        /*
         * Stale lock file.
         */
        unlink(LockFile);
        continue;
      }
      else if (((t < 0) && (errno == EPERM)) || (t == 0)) {
        /*
         * Process is still active.
         */
        unlink(tmp);
	FatalError("Server is already active for display %s\n%s %s\n%s\n",
		   port, "\tIf this server is no longer running, remove",
		   LockFile, "\tand start again.");
      }
    }
  }
  unlink(tmp);
  if (!haslock)
    FatalError("Could not create server lock file: %s\n", LockFile);
  StillLocking = FALSE;
}
