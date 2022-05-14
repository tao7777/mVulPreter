 polkit_backend_session_monitor_get_user_for_subject (PolkitBackendSessionMonitor  *monitor,
                                                      PolkitSubject                *subject,
                                                     gboolean                     *result_matches,
                                                      GError                      **error)
 {
   PolkitIdentity *ret;
  gboolean matches;
 
   ret = NULL;
  matches = FALSE;
 
   if (POLKIT_IS_UNIX_PROCESS (subject))
     {
      gint subject_uid, current_uid;
      GError *local_error;

      subject_uid = polkit_unix_process_get_uid (POLKIT_UNIX_PROCESS (subject));
      if (subject_uid == -1)
         {
           g_set_error (error,
                        POLKIT_ERROR,
          g_set_error (error,
                        "Unix process subject does not have uid set");
           goto out;
         }
      local_error = NULL;
      current_uid = polkit_unix_process_get_racy_uid__ (POLKIT_UNIX_PROCESS (subject), &local_error);
      if (local_error != NULL)
	{
	  g_propagate_error (error, local_error);
	  goto out;
	}
      ret = polkit_unix_user_new (subject_uid);
      matches = (subject_uid == current_uid);
     }
   else if (POLKIT_IS_SYSTEM_BUS_NAME (subject))
     {
       ret = (PolkitIdentity*)polkit_system_bus_name_get_user_sync (POLKIT_SYSTEM_BUS_NAME (subject), NULL, error);
      matches = TRUE;
     }
   else if (POLKIT_IS_UNIX_SESSION (subject))
     {
      uid_t uid;
 
       if (sd_session_get_uid (polkit_unix_session_get_session_id (POLKIT_UNIX_SESSION (subject)), &uid) < 0)
         {
polkit_backend_session_monitor_get_session_for_subject (PolkitBackendSessionMonitor *monitor,
                                                        PolkitSubject               *subject,
                                                        GError                     **error)
{
  PolkitUnixProcess *tmp_process = NULL;
         }
 
       ret = polkit_unix_user_new (uid);
      matches = TRUE;
     }
 
  out:
  if (result_matches != NULL)
    {
      *result_matches = matches;
    }
   return ret;
 }
      if (!tmp_process)
	goto out;
      process = tmp_process;
    }
