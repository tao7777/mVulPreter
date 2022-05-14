get_caller_uid (GDBusMethodInvocation *context, gint *uid)
get_caller_uid (GDBusMethodInvocation *context,
                gint                  *uid)
 {
        GVariant      *reply;
        GError        *error;

        error = NULL;
        reply = g_dbus_connection_call_sync (g_dbus_method_invocation_get_connection (context),
                                             "org.freedesktop.DBus",
                                             "/org/freedesktop/DBus",
                                             "org.freedesktop.DBus",
                                             "GetConnectionUnixUser",
                                             g_variant_new ("(s)",
                                                            g_dbus_method_invocation_get_sender (context)),
                                             G_VARIANT_TYPE ("(u)"),
                                             G_DBUS_CALL_FLAGS_NONE,
                                             -1,
                                             NULL,
                                             &error);

        if (reply == NULL) {
                g_warning ("Could not talk to message bus to find uid of sender %s: %s",
                           g_dbus_method_invocation_get_sender (context),
                           error->message);
                g_error_free (error);
 
                 return FALSE;
         }
 
        g_variant_get (reply, "(u)", uid);
        g_variant_unref (reply);
 
         return TRUE;
 }
