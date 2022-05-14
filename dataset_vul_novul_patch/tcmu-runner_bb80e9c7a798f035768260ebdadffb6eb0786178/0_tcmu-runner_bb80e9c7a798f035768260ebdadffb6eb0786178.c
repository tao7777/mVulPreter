on_handler_appeared(GDBusConnection *connection,
		    const gchar     *name,
		    const gchar     *name_owner,
		    gpointer         user_data)
{
	struct tcmur_handler *handler = user_data;
	struct dbus_info *info = handler->opaque;
 
 	if (info->register_invocation) {
 		info->connection = connection;
		tcmur_register_dbus_handler(handler);
 		dbus_export_handler(handler, G_CALLBACK(on_dbus_check_config));
 		g_dbus_method_invocation_return_value(info->register_invocation,
 			    g_variant_new("(bs)", TRUE, "succeeded"));
		info->register_invocation = NULL;
	}
}
