 pam_sm_close_session (pam_handle_t *pamh, int flags UNUSED,
                      int argc, const char **argv)
 {
        int i, debug = 0;
       const char *user;
       const void *data;
       const char *cookiefile;
       struct passwd *tpwd;
       uid_t fsuid;
 
       /* Try to retrieve the name of a file we created when
        * the session was opened. */
       if (pam_get_data(pamh, DATANAME, &data) != PAM_SUCCESS)
               return PAM_SUCCESS;
       cookiefile = data;

       /* Parse arguments.  We don't understand many, so
        * no sense in breaking this into a separate function. */
        for (i = 0; i < argc; i++) {
                if (strcmp(argv[i], "debug") == 0) {
                        debug = 1;
                        continue;
                }
               if (strncmp(argv[i], "xauthpath=", 10) == 0)
                        continue;
               if (strncmp(argv[i], "systemuser=", 11) == 0)
                        continue;
               if (strncmp(argv[i], "targetuser=", 11) == 0)
                        continue;
                pam_syslog(pamh, LOG_WARNING, "unrecognized option `%s'",
                       argv[i]);
        }
 
       if (pam_get_user(pamh, &user, NULL) != PAM_SUCCESS) {
               pam_syslog(pamh, LOG_ERR,
                          "error determining target user's name");
               return PAM_SESSION_ERR;
        }
       if (!(tpwd = pam_modutil_getpwnam(pamh, user))) {
               pam_syslog(pamh, LOG_ERR,
                          "error determining target user's UID");
               return PAM_SESSION_ERR;
       }

       if (debug)
               pam_syslog(pamh, LOG_DEBUG, "removing `%s'", cookiefile);
       fsuid = setfsuid(tpwd->pw_uid);
       unlink(cookiefile);
       setfsuid(fsuid);

        return PAM_SUCCESS;
 }
