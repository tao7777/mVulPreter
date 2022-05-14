  static void IBusBusNameOwnerChangedCallback(
  void IBusBusNameOwnerChanged(IBusBus* bus,
                               const gchar* name,
                               const gchar* old_name,
                               const gchar* new_name) {
     DCHECK(name);
     DCHECK(old_name);
     DCHECK(new_name);
    VLOG(1) << "Name owner is changed: name=" << name
            << ", old_name=" << old_name << ", new_name=" << new_name;
 
     if (name != std::string("org.freedesktop.IBus.Config")) {
      return;
    }

    const std::string empty_string;
    if (old_name != empty_string || new_name == empty_string) {
       LOG(WARNING) << "Unexpected name owner change: name=" << name
                    << ", old_name=" << old_name << ", new_name=" << new_name;
      // |OnConnectionChange| with false here to allow Chrome to
       return;
     }
 
    VLOG(1) << "IBus config daemon is started. Recovering ibus_config_";
 
    // successfully created, |OnConnectionChange| will be called to
    MaybeRestoreConnections();
   }
