on_handler_vanished(GDBusConnection *connection,
		    const gchar     *name,
		    gpointer         user_data)
{
	struct tcmur_handler *handler = user_data;
	struct dbus_info *info = handler->opaque;

	if (info->register_invocation) {
		char *reason;
		reason = g_strdup_printf("Cannot find handler bus name: "
				"org.kernel.TCMUService1.HandlerManager1.%s",
				handler->subtype);
		g_dbus_method_invocation_return_value(info->register_invocation,
 			    g_variant_new("(bs)", FALSE, reason));
 		g_free(reason);
 	}
	tcmur_unregister_dbus_handler(handler);
 	dbus_unexport_handler(handler);
 }
