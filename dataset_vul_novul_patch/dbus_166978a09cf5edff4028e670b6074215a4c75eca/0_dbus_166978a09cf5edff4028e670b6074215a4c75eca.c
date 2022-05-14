dbus_g_proxy_manager_filter (DBusConnection    *connection,
                             DBusMessage       *message,
                             void              *user_data)
{
  DBusGProxyManager *manager;
  
  if (dbus_message_get_type (message) != DBUS_MESSAGE_TYPE_SIGNAL)
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

  manager = user_data;

  dbus_g_proxy_manager_ref (manager);
  
  LOCK_MANAGER (manager);
  
  if (dbus_message_is_signal (message,
                              DBUS_INTERFACE_LOCAL,
                              "Disconnected"))
    {
      /* Destroy all the proxies, quite possibly resulting in unreferencing
       * the proxy manager and the connection as well.
       */
      GSList *all;
      GSList *tmp;

      all = dbus_g_proxy_manager_list_all (manager);

      tmp = all;
      while (tmp != NULL)
        {
          DBusGProxy *proxy;

          proxy = DBUS_G_PROXY (tmp->data);

          UNLOCK_MANAGER (manager);
          dbus_g_proxy_destroy (proxy);
          g_object_unref (G_OBJECT (proxy));
          LOCK_MANAGER (manager);
          
          tmp = tmp->next;
        }

      g_slist_free (all);

#ifndef G_DISABLE_CHECKS
      if (manager->proxy_lists != NULL)
        g_warning ("Disconnection emitted \"destroy\" on all DBusGProxy, but somehow new proxies were created in response to one of those destroy signals. This will cause a memory leak.");
#endif
    }
  else
    {
      char *tri;
      GSList *full_list;
      GSList *owned_names;
       GSList *tmp;
       const char *sender;
 
      sender = dbus_message_get_sender (message);

       /* First we handle NameOwnerChanged internally */
      if (g_strcmp0 (sender, DBUS_SERVICE_DBUS) == 0 &&
	  dbus_message_is_signal (message,
 				  DBUS_INTERFACE_DBUS,
 				  "NameOwnerChanged"))
 	{
	  DBusError derr;

	  dbus_error_init (&derr);
	  if (!dbus_message_get_args (message,
				      &derr,
				      DBUS_TYPE_STRING,
				      &name,
				      DBUS_TYPE_STRING,
				      &prev_owner,
				      DBUS_TYPE_STRING,
				      &new_owner,
				      DBUS_TYPE_INVALID))
	    {
	      /* Ignore this error */
	      dbus_error_free (&derr);
	    }
	  else if (manager->owner_names != NULL)
	    {
	      dbus_g_proxy_manager_replace_name_owner (manager, name, prev_owner, new_owner);
	    }
	}

 	    }
 	}
