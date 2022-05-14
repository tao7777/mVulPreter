  static void IBusBusNameOwnerChangedCallback(
      IBusBus* bus,
      const gchar* name, const gchar* old_name, const gchar* new_name,
      gpointer user_data) {
     DCHECK(name);
     DCHECK(old_name);
     DCHECK(new_name);
    DLOG(INFO) << "Name owner is changed: name=" << name
               << ", old_name=" << old_name << ", new_name=" << new_name;
 
     if (name != std::string("org.freedesktop.IBus.Config")) {
      return;
    }

    const std::string empty_string;
    if (old_name != empty_string || new_name == empty_string) {
       LOG(WARNING) << "Unexpected name owner change: name=" << name
                    << ", old_name=" << old_name << ", new_name=" << new_name;
       return;
     }
 
    LOG(INFO) << "IBus config daemon is started. Recovering ibus_config_";
    g_return_if_fail(user_data);
    InputMethodStatusConnection* self
        = static_cast<InputMethodStatusConnection*>(user_data);
 
    self->MaybeRestoreConnections();
   }
