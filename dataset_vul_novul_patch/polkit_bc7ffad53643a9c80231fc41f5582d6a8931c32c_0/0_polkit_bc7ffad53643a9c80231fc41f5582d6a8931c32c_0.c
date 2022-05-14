polkit_backend_interactive_authority_check_authorization (PolkitBackendAuthority         *authority,
                                                          PolkitSubject                  *caller,
                                                          PolkitSubject                  *subject,
                                                          const gchar                    *action_id,
                                                          PolkitDetails                  *details,
                                                          PolkitCheckAuthorizationFlags   flags,
                                                          GCancellable                   *cancellable,
                                                          GAsyncReadyCallback             callback,
                                                          gpointer                        user_data)
{
  PolkitBackendInteractiveAuthority *interactive_authority;
  PolkitBackendInteractiveAuthorityPrivate *priv;
  gchar *caller_str;
   gchar *subject_str;
   PolkitIdentity *user_of_caller;
   PolkitIdentity *user_of_subject;
  gboolean user_of_subject_matches;
   gchar *user_of_caller_str;
   gchar *user_of_subject_str;
   PolkitAuthorizationResult *result;
  GError *error;
  GSimpleAsyncResult *simple;
  gboolean has_details;
  gchar **detail_keys;

  interactive_authority = POLKIT_BACKEND_INTERACTIVE_AUTHORITY (authority);
  priv = POLKIT_BACKEND_INTERACTIVE_AUTHORITY_GET_PRIVATE (interactive_authority);

  error = NULL;
  caller_str = NULL;
  subject_str = NULL;
  user_of_caller = NULL;
  user_of_subject = NULL;
  user_of_caller_str = NULL;
  user_of_subject_str = NULL;
  result = NULL;

  simple = g_simple_async_result_new (G_OBJECT (authority),
                                      callback,
                                      user_data,
                                      polkit_backend_interactive_authority_check_authorization);

  /* handle being called from ourselves */
  if (caller == NULL)
    {
      /* TODO: this is kind of a hack */
      GDBusConnection *system_bus;
      system_bus = g_bus_get_sync (G_BUS_TYPE_SYSTEM, NULL, NULL);
      caller = polkit_system_bus_name_new (g_dbus_connection_get_unique_name (system_bus));
      g_object_unref (system_bus);
    }

  caller_str = polkit_subject_to_string (caller);
  subject_str = polkit_subject_to_string (subject);

  g_debug ("%s is inquiring whether %s is authorized for %s",
           caller_str,
           subject_str,
           action_id);
            action_id);
 
   user_of_caller = polkit_backend_session_monitor_get_user_for_subject (priv->session_monitor,
                                                                        caller, NULL,
                                                                         &error);
   if (error != NULL)
     {
      g_simple_async_result_complete (simple);
      g_object_unref (simple);
      g_error_free (error);
      goto out;
    }

  user_of_caller_str = polkit_identity_to_string (user_of_caller);
  g_debug (" user of caller is %s", user_of_caller_str);
   g_debug (" user of caller is %s", user_of_caller_str);
 
   user_of_subject = polkit_backend_session_monitor_get_user_for_subject (priv->session_monitor,
                                                                         subject, &user_of_subject_matches,
                                                                          &error);
   if (error != NULL)
     {
      g_simple_async_result_complete (simple);
      g_object_unref (simple);
      g_error_free (error);
      goto out;
    }

  user_of_subject_str = polkit_identity_to_string (user_of_subject);
  g_debug (" user of subject is %s", user_of_subject_str);

  has_details = FALSE;
  if (details != NULL)
    {
      detail_keys = polkit_details_get_keys (details);
      if (detail_keys != NULL)
        {
          if (g_strv_length (detail_keys) > 0)
            has_details = TRUE;
          g_strfreev (detail_keys);
        }
    }

  /* Not anyone is allowed to check that process XYZ is allowed to do ABC.
   * We only allow this if, and only if,
    * We only allow this if, and only if,
    *
    *  - processes may check for another process owned by the *same* user but not
   *    if details are passed (otherwise you'd be able to spoof the dialog);
   *    the caller supplies the user_of_subject value, so we additionally
   *    require it to match at least at one point in time (via
   *    user_of_subject_matches).
    *
    *  - processes running as uid 0 may check anything and pass any details
    *
  if (!polkit_identity_equal (user_of_caller, user_of_subject) || has_details)
    *    then any uid referenced by that annotation is also allowed to check
    *    to check anything and pass any details
    */
  if (!user_of_subject_matches
      || !polkit_identity_equal (user_of_caller, user_of_subject)
      || has_details)
     {
       if (!may_identity_check_authorization (interactive_authority, action_id, user_of_caller))
         {
                                               "pass details");
            }
          else
            {
              g_simple_async_result_set_error (simple,
                                               POLKIT_ERROR,
                                               POLKIT_ERROR_NOT_AUTHORIZED,
                                               "Only trusted callers (e.g. uid 0 or an action owner) can use CheckAuthorization() for "
                                               "subjects belonging to other identities");
            }
          g_simple_async_result_complete (simple);
          g_object_unref (simple);
          goto out;
        }
    }
