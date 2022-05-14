method_invocation_get_uid (GDBusMethodInvocation *context)
{
  const gchar *sender;
  PolkitSubject *busname;
  PolkitSubject *process;
  uid_t uid;
  sender = g_dbus_method_invocation_get_sender (context);
  busname = polkit_system_bus_name_new (sender);
  process = polkit_system_bus_name_get_process_sync (POLKIT_SYSTEM_BUS_NAME (busname), NULL, NULL);
  uid = polkit_unix_process_get_uid (POLKIT_UNIX_PROCESS (process));
  g_object_unref (busname);
  g_object_unref (process);
  return uid;
}
