 pam_sm_close_session (pam_handle_t *pamh, int flags UNUSED,
                      int argc, const char **argv)
 {
       void *cookiefile;
        int i, debug = 0;
       const char* user;
       struct passwd *tpwd = NULL;
       uid_t unlinkuid, fsuid;
       if (pam_get_user(pamh, &user, NULL) != PAM_SUCCESS)
               pam_syslog(pamh, LOG_ERR, "error determining target user's name");
       else {
         tpwd = pam_modutil_getpwnam(pamh, user);
         if (!tpwd)
           pam_syslog(pamh, LOG_ERR, "error determining target user's UID");
         else
           unlinkuid = tpwd->pw_uid;
       }
 
       /* Parse arguments.  We don't understand many, so no sense in breaking
        * this into a separate function. */
        for (i = 0; i < argc; i++) {
                if (strcmp(argv[i], "debug") == 0) {
                        debug = 1;
                        continue;
                }
               if (strncmp(argv[i], "xauthpath=", 10) == 0) {
                        continue;
               }
               if (strncmp(argv[i], "systemuser=", 11) == 0) {
                        continue;
               }
               if (strncmp(argv[i], "targetuser=", 11) == 0) {
                        continue;
               }
                pam_syslog(pamh, LOG_WARNING, "unrecognized option `%s'",
                       argv[i]);
        }
 
       /* Try to retrieve the name of a file we created when the session was
        * opened. */
       if (pam_get_data(pamh, DATANAME, (const void**) &cookiefile) == PAM_SUCCESS) {
               /* We'll only try to remove the file once. */
               if (strlen((char*)cookiefile) > 0) {
                       if (debug) {
                               pam_syslog(pamh, LOG_DEBUG, "removing `%s'",
                                      (char*)cookiefile);
                       }
                       /* NFS with root_squash requires non-root user */
                       if (tpwd)
                               fsuid = setfsuid(unlinkuid);
                       unlink((char*)cookiefile);
                       if (tpwd)
                               setfsuid(fsuid);
                       *((char*)cookiefile) = '\0';
               }
        }
        return PAM_SUCCESS;
 }
