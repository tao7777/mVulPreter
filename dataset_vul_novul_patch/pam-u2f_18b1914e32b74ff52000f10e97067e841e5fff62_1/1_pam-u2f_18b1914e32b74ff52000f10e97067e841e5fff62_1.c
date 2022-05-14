int get_devices_from_authfile(const char *authfile, const char *username,
                              unsigned max_devs, int verbose, FILE *debug_file,
                              device_t *devices, unsigned *n_devs) {

  char *buf = NULL;
  char *s_user, *s_token;
  int retval = 0;
  int fd = -1;
  struct stat st;
  struct passwd *pw = NULL, pw_s;
  char buffer[BUFSIZE];
  int gpu_ret;
  FILE *opwfile = NULL;
  unsigned i, j;

   /* Ensure we never return uninitialized count. */
   *n_devs = 0;
 
  fd = open(authfile, O_RDONLY, 0);
   if (fd < 0) {
     if (verbose)
       D(debug_file, "Cannot open file: %s (%s)", authfile, strerror(errno));
    goto err;
  }

  if (fstat(fd, &st) < 0) {
    if (verbose)
      D(debug_file, "Cannot stat file: %s (%s)", authfile, strerror(errno));
    goto err;
  }

  if (!S_ISREG(st.st_mode)) {
    if (verbose)
      D(debug_file, "%s is not a regular file", authfile);
    goto err;
  }

  if (st.st_size == 0) {
    if (verbose)
      D(debug_file, "File %s is empty", authfile);
    goto err;
  }

  gpu_ret = getpwuid_r(st.st_uid, &pw_s, buffer, sizeof(buffer), &pw);
  if (gpu_ret != 0 || pw == NULL) {
    D(debug_file, "Unable to retrieve credentials for uid %u, (%s)", st.st_uid,
       strerror(errno));
    goto err;
  }

  if (strcmp(pw->pw_name, username) != 0 && strcmp(pw->pw_name, "root") != 0) {
    if (strcmp(username, "root") != 0) {
      D(debug_file, "The owner of the authentication file is neither %s nor root",
         username);
    } else {
      D(debug_file, "The owner of the authentication file is not root");
    }
    goto err;
  }

  opwfile = fdopen(fd, "r");
  if (opwfile == NULL) {
     if (verbose)
       D(debug_file, "fdopen: %s", strerror(errno));
     goto err;
   }
 
   buf = malloc(sizeof(char) * (DEVSIZE * max_devs));
  if (!buf) {
    if (verbose)
      D(debug_file, "Unable to allocate memory");
    goto err;
  }

  retval = -2;
  while (fgets(buf, (int)(DEVSIZE * (max_devs - 1)), opwfile)) {
    char *saveptr = NULL;
    if (buf[strlen(buf) - 1] == '\n')
      buf[strlen(buf) - 1] = '\0';

    if (verbose)
      D(debug_file, "Authorization line: %s", buf);

    s_user = strtok_r(buf, ":", &saveptr);
    if (s_user && strcmp(username, s_user) == 0) {
      if (verbose)
        D(debug_file, "Matched user: %s", s_user);

      retval = -1; // We found at least one line for the user

      for (i = 0; i < *n_devs; i++) {
        free(devices[i].keyHandle);
        free(devices[i].publicKey);
        devices[i].keyHandle = NULL;
        devices[i].publicKey = NULL;
      }
      *n_devs = 0;

      i = 0;
      while ((s_token = strtok_r(NULL, ",", &saveptr))) {
        devices[i].keyHandle = NULL;
        devices[i].publicKey = NULL;

        if ((*n_devs)++ > MAX_DEVS - 1) {
          *n_devs = MAX_DEVS;
          if (verbose)
            D(debug_file, "Found more than %d devices, ignoring the remaining ones",
               MAX_DEVS);
          break;
        }

        if (verbose)
          D(debug_file, "KeyHandle for device number %d: %s", i + 1, s_token);

        devices[i].keyHandle = strdup(s_token);

        if (!devices[i].keyHandle) {
          if (verbose)
            D(debug_file, "Unable to allocate memory for keyHandle number %d", i);
          goto err;
        }

        s_token = strtok_r(NULL, ":", &saveptr);

        if (!s_token) {
          if (verbose)
            D(debug_file, "Unable to retrieve publicKey number %d", i + 1);
          goto err;
        }

        if (verbose)
          D(debug_file, "publicKey for device number %d: %s", i + 1, s_token);

        if (strlen(s_token) % 2 != 0) {
          if (verbose)
            D(debug_file, "Length of key number %d not even", i + 1);
          goto err;
        }

        devices[i].key_len = strlen(s_token) / 2;

        if (verbose)
          D(debug_file, "Length of key number %d is %zu", i + 1, devices[i].key_len);

        devices[i].publicKey =
          malloc((sizeof(unsigned char) * devices[i].key_len));

        if (!devices[i].publicKey) {
          if (verbose)
            D(debug_file, "Unable to allocate memory for publicKey number %d", i);
          goto err;
        }

        for (j = 0; j < devices[i].key_len; j++) {
          unsigned int x;
          if (sscanf(&s_token[2 * j], "%2x", &x) != 1) {
            if (verbose)
              D(debug_file, "Invalid hex number in key");
            goto err;
          }
          devices[i].publicKey[j] = (unsigned char)x;
        }

        i++;
      }
    }
  }

  if (verbose)
    D(debug_file, "Found %d device(s) for user %s", *n_devs, username);

  retval = 1;
  goto out;

err:
  for (i = 0; i < *n_devs; i++) {
    free(devices[i].keyHandle);
    free(devices[i].publicKey);
    devices[i].keyHandle = NULL;
    devices[i].publicKey = NULL;
  }

  *n_devs = 0;

out:
  if (buf) {
    free(buf);
    buf = NULL;
  }
 
   if (opwfile)
     fclose(opwfile);
  else if (fd >= 0)
     close(fd);
   return retval;
 }
