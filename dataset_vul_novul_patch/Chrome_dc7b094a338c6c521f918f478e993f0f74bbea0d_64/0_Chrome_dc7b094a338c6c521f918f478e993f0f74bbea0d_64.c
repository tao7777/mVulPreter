  InputMethodStatusConnection()
  // Functions that end with Thunk are used to deal with glib callbacks.
  //
  // Note that we cannot use CHROMEG_CALLBACK_0() here as we'll define
  // IBusBusConnected() inline. If we are to define the function outside
  // of the class definition, we should use CHROMEG_CALLBACK_0() here.
  //
  // CHROMEG_CALLBACK_0(Impl,
  //                    void, IBusBusConnected, IBusBus*);
  static void IBusBusConnectedThunk(IBusBus* sender, gpointer userdata) {
    return reinterpret_cast<IBusControllerImpl*>(userdata)
        ->IBusBusConnected(sender);
  }
  static void IBusBusDisconnectedThunk(IBusBus* sender, gpointer userdata) {
    return reinterpret_cast<IBusControllerImpl*>(userdata)
        ->IBusBusDisconnected(sender);
  }
  static void IBusBusGlobalEngineChangedThunk(IBusBus* sender,
                                              const gchar* engine_name,
                                              gpointer userdata) {
    return reinterpret_cast<IBusControllerImpl*>(userdata)
        ->IBusBusGlobalEngineChanged(sender, engine_name);
  }
  static void IBusBusNameOwnerChangedThunk(IBusBus* sender,
                                           const gchar* name,
                                           const gchar* old_name,
                                           const gchar* new_name,
                                           gpointer userdata) {
    return reinterpret_cast<IBusControllerImpl*>(userdata)
        ->IBusBusNameOwnerChanged(sender, name, old_name, new_name);
  }
  static void FocusInThunk(IBusPanelService* sender,
                           const gchar* input_context_path,
                           gpointer userdata) {
    return reinterpret_cast<IBusControllerImpl*>(userdata)
        ->FocusIn(sender, input_context_path);
  }
  static void RegisterPropertiesThunk(IBusPanelService* sender,
                                      IBusPropList* prop_list,
                                      gpointer userdata) {
    return reinterpret_cast<IBusControllerImpl*>(userdata)
        ->RegisterProperties(sender, prop_list);
  }
  static void UpdatePropertyThunk(IBusPanelService* sender,
                                  IBusProperty* ibus_prop,
                                  gpointer userdata) {
    return reinterpret_cast<IBusControllerImpl*>(userdata)
        ->UpdateProperty(sender, ibus_prop);
  }

  friend struct DefaultSingletonTraits<IBusControllerImpl>;
  IBusControllerImpl()
      : ibus_(NULL),
         ibus_config_(NULL) {
   }
