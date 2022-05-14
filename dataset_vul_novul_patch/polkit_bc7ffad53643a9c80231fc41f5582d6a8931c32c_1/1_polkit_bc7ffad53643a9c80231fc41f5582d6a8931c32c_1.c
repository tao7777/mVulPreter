 polkit_backend_session_monitor_get_user_for_subject (PolkitBackendSessionMonitor  *monitor,
                                                      PolkitSubject                *subject,
                                                      GError                      **error)
 {
   PolkitIdentity *ret;
  guint32 uid;
 
   ret = NULL;
 
   if (POLKIT_IS_UNIX_PROCESS (subject))
     {
      uid = polkit_unix_process_get_uid (POLKIT_UNIX_PROCESS (subject));
      if ((gint) uid == -1)
         {
           g_set_error (error,
                        POLKIT_ERROR,
          g_set_error (error,
                        "Unix process subject does not have uid set");
           goto out;
         }
      ret = polkit_unix_user_new (uid);
     }
   else if (POLKIT_IS_SYSTEM_BUS_NAME (subject))
     {
       ret = (PolkitIdentity*)polkit_system_bus_name_get_user_sync (POLKIT_SYSTEM_BUS_NAME (subject), NULL, error);
     }
   else if (POLKIT_IS_UNIX_SESSION (subject))
     {
 
       if (sd_session_get_uid (polkit_unix_session_get_session_id (POLKIT_UNIX_SESSION (subject)), &uid) < 0)
         {
polkit_backend_session_monitor_get_session_for_subject (PolkitBackendSessionMonitor *monitor,
                                                        PolkitSubject               *subject,
                                                        GError                     **error)
{
  PolkitUnixProcess *tmp_process = NULL;
         }
 
       ret = polkit_unix_user_new (uid);
     }
 
  out:
   return ret;
 }
      if (!tmp_process)
	goto out;
      process = tmp_process;
    }
