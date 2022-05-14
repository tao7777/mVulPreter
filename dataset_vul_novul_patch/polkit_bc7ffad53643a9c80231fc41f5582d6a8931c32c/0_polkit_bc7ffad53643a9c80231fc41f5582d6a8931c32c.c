log_result (PolkitBackendInteractiveAuthority    *authority,
            const gchar                          *action_id,
            PolkitSubject                        *subject,
            PolkitSubject                        *caller,
            PolkitAuthorizationResult            *result)
{
  PolkitBackendInteractiveAuthorityPrivate *priv;
  PolkitIdentity *user_of_subject;
  const gchar *log_result_str;
  gchar *subject_str;
  gchar *user_of_subject_str;
  gchar *caller_str;
  gchar *subject_cmdline;
  gchar *caller_cmdline;

  priv = POLKIT_BACKEND_INTERACTIVE_AUTHORITY_GET_PRIVATE (authority);

  log_result_str = "DENYING";
   if (polkit_authorization_result_get_is_authorized (result))
     log_result_str = "ALLOWING";
 
  user_of_subject = polkit_backend_session_monitor_get_user_for_subject (priv->session_monitor, subject, NULL, NULL);
 
   subject_str = polkit_subject_to_string (subject);
 
  if (user_of_subject != NULL)
    user_of_subject_str = polkit_identity_to_string (user_of_subject);
  else
    user_of_subject_str = g_strdup ("<unknown>");
  caller_str = polkit_subject_to_string (caller);

  subject_cmdline = _polkit_subject_get_cmdline (subject);
  if (subject_cmdline == NULL)
    subject_cmdline = g_strdup ("<unknown>");

  caller_cmdline = _polkit_subject_get_cmdline (caller);
  if (caller_cmdline == NULL)
    caller_cmdline = g_strdup ("<unknown>");

  polkit_backend_authority_log (POLKIT_BACKEND_AUTHORITY (authority),
                                "%s action %s for %s [%s] owned by %s (check requested by %s [%s])",
                                log_result_str,
                                action_id,
                                subject_str,
                                subject_cmdline,
                                user_of_subject_str,
                                caller_str,
                                caller_cmdline);

  if (user_of_subject != NULL)
    g_object_unref (user_of_subject);
  g_free (subject_str);
  g_free (user_of_subject_str);
  g_free (caller_str);
  g_free (subject_cmdline);
  g_free (caller_cmdline);
}
