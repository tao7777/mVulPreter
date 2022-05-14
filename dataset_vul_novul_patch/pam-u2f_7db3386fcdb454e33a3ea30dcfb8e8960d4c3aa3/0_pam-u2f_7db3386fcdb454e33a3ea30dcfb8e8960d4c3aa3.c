int pam_sm_authenticate(pam_handle_t *pamh, int flags, int argc,
                        const char **argv) {

  struct passwd *pw = NULL, pw_s;
  const char *user = NULL;

  cfg_t cfg_st;
  cfg_t *cfg = &cfg_st;
  char buffer[BUFSIZE];
  char *buf = NULL;
  char *authfile_dir;
  size_t authfile_dir_len;
  int pgu_ret, gpn_ret;
   int retval = PAM_IGNORE;
   device_t *devices = NULL;
   unsigned n_devices = 0;
  int openasuser = 0;
   int should_free_origin = 0;
   int should_free_appid = 0;
   int should_free_auth_file = 0;
   int should_free_authpending_file = 0;
  PAM_MODUTIL_DEF_PRIVS(privs);
 
   parse_cfg(flags, argc, argv, cfg);
 
  if (!cfg->origin) {
    strcpy(buffer, DEFAULT_ORIGIN_PREFIX);

    if (gethostname(buffer + strlen(DEFAULT_ORIGIN_PREFIX),
                    BUFSIZE - strlen(DEFAULT_ORIGIN_PREFIX)) == -1) {
      DBG("Unable to get host name");
      goto done;
    }
    DBG("Origin not specified, using \"%s\"", buffer);
    cfg->origin = strdup(buffer);
    if (!cfg->origin) {
      DBG("Unable to allocate memory");
      goto done;
    } else {
      should_free_origin = 1;
    }
  }

  if (!cfg->appid) {
    DBG("Appid not specified, using the same value of origin (%s)",
         cfg->origin);
    cfg->appid = strdup(cfg->origin);
    if (!cfg->appid) {
      DBG("Unable to allocate memory")
      goto done;
    } else {
      should_free_appid = 1;
    }
  }

  if (cfg->max_devs == 0) {
    DBG("Maximum devices number not set. Using default (%d)", MAX_DEVS);
    cfg->max_devs = MAX_DEVS;
  }

  devices = malloc(sizeof(device_t) * cfg->max_devs);
  if (!devices) {
    DBG("Unable to allocate memory");
    retval = PAM_IGNORE;
    goto done;
  }

  pgu_ret = pam_get_user(pamh, &user, NULL);
  if (pgu_ret != PAM_SUCCESS || user == NULL) {
    DBG("Unable to access user %s", user);
    retval = PAM_CONV_ERR;
    goto done;
  }

  DBG("Requesting authentication for user %s", user);

  gpn_ret = getpwnam_r(user, &pw_s, buffer, sizeof(buffer), &pw);
  if (gpn_ret != 0 || pw == NULL || pw->pw_dir == NULL ||
      pw->pw_dir[0] != '/') {
    DBG("Unable to retrieve credentials for user %s, (%s)", user,
         strerror(errno));
    retval = PAM_USER_UNKNOWN;
    goto done;
  }

  DBG("Found user %s", user);
  DBG("Home directory for %s is %s", user, pw->pw_dir);

  if (!cfg->auth_file) {
    buf = NULL;
    authfile_dir = secure_getenv(DEFAULT_AUTHFILE_DIR_VAR);
    if (!authfile_dir) {
      DBG("Variable %s is not set. Using default value ($HOME/.config/)",
           DEFAULT_AUTHFILE_DIR_VAR);
      authfile_dir_len =
        strlen(pw->pw_dir) + strlen("/.config") + strlen(DEFAULT_AUTHFILE) + 1;
      buf = malloc(sizeof(char) * (authfile_dir_len));

      if (!buf) {
        DBG("Unable to allocate memory");
        retval = PAM_IGNORE;
         goto done;
       }
 
      /* Opening a file in a users $HOME, need to drop privs for security */
      openasuser = geteuid() == 0 ? 1 : 0;

       snprintf(buf, authfile_dir_len,
                "%s/.config%s", pw->pw_dir, DEFAULT_AUTHFILE);
     } else {
      DBG("Variable %s set to %s", DEFAULT_AUTHFILE_DIR_VAR, authfile_dir);
      authfile_dir_len = strlen(authfile_dir) + strlen(DEFAULT_AUTHFILE) + 1;
      buf = malloc(sizeof(char) * (authfile_dir_len));

      if (!buf) {
        DBG("Unable to allocate memory");
        retval = PAM_IGNORE;
        goto done;
      }
 
       snprintf(buf, authfile_dir_len,
                "%s%s", authfile_dir, DEFAULT_AUTHFILE);

      if (!openasuser) {
	DBG("WARNING: not dropping privileges when reading %s, please "
	    "consider setting openasuser=1 in the module configuration", buf);
      }
     }
 
    DBG("Using authentication file %s", buf);
 
     cfg->auth_file = buf; /* cfg takes ownership */
     should_free_auth_file = 1;
    buf = NULL;
  } else {
     DBG("Using authentication file %s", cfg->auth_file);
   }
 
  if (!openasuser) {
    openasuser = geteuid() == 0 && cfg->openasuser;
  }
   if (openasuser) {
    DBG("Dropping privileges");
    if (pam_modutil_drop_priv(pamh, &privs, pw)) {
      DBG("Unable to switch user to uid %i", pw->pw_uid);
       retval = PAM_IGNORE;
       goto done;
     }
    DBG("Switched to uid %i", pw->pw_uid);
   }
   retval = get_devices_from_authfile(cfg->auth_file, user, cfg->max_devs,
                                      cfg->debug, cfg->debug_file,
                                      devices, &n_devices);
   if (openasuser) {
    if (pam_modutil_regain_priv(pamh, &privs)) {
      DBG("could not restore privileges");
       retval = PAM_IGNORE;
       goto done;
     }
    DBG("Restored privileges");
   }
 
   if (retval != 1) {
    n_devices = 0;
  }

  if (n_devices == 0) {
    if (cfg->nouserok) {
      DBG("Found no devices but nouserok specified. Skipping authentication");
      retval = PAM_SUCCESS;
      goto done;
    } else if (retval != 1) {
      DBG("Unable to get devices from file %s", cfg->auth_file);
      retval = PAM_AUTHINFO_UNAVAIL;
      goto done;
    } else {
      DBG("Found no devices. Aborting.");
      retval = PAM_AUTHINFO_UNAVAIL;
      goto done;
    }
  }

  if (!cfg->authpending_file) {
    int actual_size = snprintf(buffer, BUFSIZE, DEFAULT_AUTHPENDING_FILE_PATH, getuid());
    if (actual_size >= 0 && actual_size < BUFSIZE) {
      cfg->authpending_file = strdup(buffer);
    }
    if (!cfg->authpending_file) {
      DBG("Unable to allocate memory for the authpending_file, touch request notifications will not be emitted");
    } else {
      should_free_authpending_file = 1;
    }
  } else {
    if (strlen(cfg->authpending_file) == 0) {
      DBG("authpending_file is set to an empty value, touch request notifications will be disabled");
      cfg->authpending_file = NULL;
    }
  }

  int authpending_file_descriptor = -1;
  if (cfg->authpending_file) {
    DBG("Using file '%s' for emitting touch request notifications", cfg->authpending_file);

    authpending_file_descriptor =
      open(cfg->authpending_file, O_RDONLY | O_CREAT | O_CLOEXEC | O_NOFOLLOW | O_NOCTTY, 0664);
    if (authpending_file_descriptor < 0) {
      DBG("Unable to emit 'authentication started' notification by opening the file '%s', (%s)",
          cfg->authpending_file, strerror(errno));
    }
  }

  if (cfg->manual == 0) {
    if (cfg->interactive) {
      converse(pamh, PAM_PROMPT_ECHO_ON,
               cfg->prompt != NULL ? cfg->prompt : DEFAULT_PROMPT);
    }

    retval = do_authentication(cfg, devices, n_devices, pamh);
  } else {
    retval = do_manual_authentication(cfg, devices, n_devices, pamh);
  }

  if (authpending_file_descriptor >= 0) {
    if (close(authpending_file_descriptor) < 0) {
      DBG("Unable to emit 'authentication stopped' notification by closing the file '%s', (%s)",
          cfg->authpending_file, strerror(errno));
    }
  }

  if (retval != 1) {
    DBG("do_authentication returned %d", retval);
    retval = PAM_AUTH_ERR;
    goto done;
  }

  retval = PAM_SUCCESS;

done:
  free_devices(devices, n_devices);

  if (buf) {
    free(buf);
    buf = NULL;
  }

  if (should_free_origin) {
    free((char *) cfg->origin);
    cfg->origin = NULL;
  }

  if (should_free_appid) {
    free((char *) cfg->appid);
    cfg->appid = NULL;
  }

  if (should_free_auth_file) {
    free((char *) cfg->auth_file);
    cfg->auth_file = NULL;
  }

  if (should_free_authpending_file) {
    free((char *) cfg->authpending_file);
    cfg->authpending_file = NULL;
  }

  if (cfg->alwaysok && retval != PAM_SUCCESS) {
    DBG("alwaysok needed (otherwise return with %d)", retval);
    retval = PAM_SUCCESS;
  }
  DBG("done. [%s]", pam_strerror(pamh, retval));

  if (cfg->is_custom_debug_file) {
    fclose(cfg->debug_file);
  }

  return retval;
}
