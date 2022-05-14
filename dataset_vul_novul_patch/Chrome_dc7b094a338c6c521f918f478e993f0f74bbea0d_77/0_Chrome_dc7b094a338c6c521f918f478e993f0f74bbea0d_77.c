  ~InputMethodStatusConnection() {
  ~IBusControllerImpl() {
  }

  bool IBusConnectionsAreAlive() {
    return ibus_ && ibus_bus_is_connected(ibus_) && ibus_config_;
  }

  void MaybeRestoreConnections() {
    if (IBusConnectionsAreAlive()) {
      return;
    }
    MaybeCreateIBus();
     MaybeRestoreIBusConfig();
     if (IBusConnectionsAreAlive()) {
       ConnectPanelServiceSignals();
      VLOG(1) << "Notifying Chrome that IBus is ready.";
      FOR_EACH_OBSERVER(Observer, observers_, OnConnectionChange(true));
     }
   }
 
  void MaybeCreateIBus() {
    if (ibus_) {
      return;
    }

    ibus_init();
    ibus_ = ibus_bus_new();

    if (!ibus_) {
      LOG(ERROR) << "ibus_bus_new() failed";
      return;
    }
    ConnectIBusSignals();

    ibus_bus_set_watch_dbus_signal(ibus_, TRUE);
     ibus_bus_set_watch_ibus_signal(ibus_, TRUE);
 
     if (ibus_bus_is_connected(ibus_)) {
      VLOG(1) << "IBus connection is ready.";
     }
   }
 
  void MaybeRestoreIBusConfig() {
    if (!ibus_) {
      return;
    }

    MaybeDestroyIBusConfig();

     if (!ibus_config_) {
       GDBusConnection* ibus_connection = ibus_bus_get_connection(ibus_);
       if (!ibus_connection) {
        VLOG(1) << "Couldn't create an ibus config object since "
                << "IBus connection is not ready.";
         return;
       }
       const gboolean disconnected
          = g_dbus_connection_is_closed(ibus_connection);
      if (disconnected) {
        LOG(ERROR) << "Couldn't create an ibus config object since "
                   << "IBus connection is closed.";
        return;
      }
      ibus_config_ = ibus_config_new(ibus_connection,
                                     NULL /* do not cancel the operation */,
                                     NULL /* do not get error information */);
      if (!ibus_config_) {
        LOG(ERROR) << "ibus_config_new() failed. ibus-memconf is not ready?";
        return;
      }

       g_object_ref(ibus_config_);
      VLOG(1) << "ibus_config_ is ready.";
     }
   }
 
  void MaybeDestroyIBusConfig() {
    if (!ibus_) {
      LOG(ERROR) << "MaybeDestroyIBusConfig: ibus_ is NULL";
      return;
    }
    if (ibus_config_ && !ibus_bus_is_connected(ibus_)) {
      g_object_unref(ibus_config_);
      ibus_config_ = NULL;
     }
   }
 
  void DoRegisterProperties(IBusPropList* ibus_prop_list) {
    VLOG(1) << "RegisterProperties" << (ibus_prop_list ? "" : " (clear)");
 
     ImePropertyList prop_list;  // our representation.
     if (ibus_prop_list) {
       if (!FlattenPropertyList(ibus_prop_list, &prop_list)) {
        DoRegisterProperties(NULL);
         return;
       }
     }
    VLOG(1) << "RegisterProperties" << (ibus_prop_list ? "" : " (clear)");
    FOR_EACH_OBSERVER(Observer, observers_,
                      OnRegisterImeProperties(prop_list));
   }
 
  void UpdateUI(const char* current_global_engine_id) {
    DCHECK(current_global_engine_id);

    const IBusEngineInfo* engine_info = NULL;
    for (size_t i = 0; i < arraysize(kIBusEngines); ++i) {
      if (kIBusEngines[i].name == std::string(current_global_engine_id)) {
        engine_info = &kIBusEngines[i];
        break;
      }
    }

    if (!engine_info) {
      LOG(ERROR) << current_global_engine_id
                 << " is not found in the input method white-list.";
      return;
    }

    InputMethodDescriptor current_input_method =
        CreateInputMethodDescriptor(engine_info->name,
                                    engine_info->longname,
                                     engine_info->layout,
                                     engine_info->language);
 
    VLOG(1) << "Updating the UI. ID:" << current_input_method.id
            << ", keyboard_layout:" << current_input_method.keyboard_layout;
 
    FOR_EACH_OBSERVER(Observer, observers_,
                      OnCurrentInputMethodChanged(current_input_method));
   }
 
  void ConnectIBusSignals() {
    if (!ibus_) {
      return;
    }

     g_signal_connect_after(ibus_,
                            "connected",
                           G_CALLBACK(IBusBusConnectedThunk),
                            this);
 
     g_signal_connect(ibus_,
                      "disconnected",
                     G_CALLBACK(IBusBusDisconnectedThunk),
                      this);
     g_signal_connect(ibus_,
                      "global-engine-changed",
                     G_CALLBACK(IBusBusGlobalEngineChangedThunk),
                      this);
     g_signal_connect(ibus_,
                      "name-owner-changed",
                     G_CALLBACK(IBusBusNameOwnerChangedThunk),
                      this);
   }
 
  void ConnectPanelServiceSignals() {
    if (!ibus_) {
      return;
    }

    IBusPanelService* ibus_panel_service = IBUS_PANEL_SERVICE(
        g_object_get_data(G_OBJECT(ibus_), kPanelObjectKey));
    if (!ibus_panel_service) {
      LOG(ERROR) << "IBusPanelService is NOT available.";
      return;
    }
 
     g_signal_connect(ibus_panel_service,
                      "focus-in",
                     G_CALLBACK(FocusInThunk),
                      this);
     g_signal_connect(ibus_panel_service,
                      "register-properties",
                     G_CALLBACK(RegisterPropertiesThunk),
                      this);
     g_signal_connect(ibus_panel_service,
                      "update-property",
                     G_CALLBACK(UpdatePropertyThunk),
                      this);
   }
 
  void IBusBusConnected(IBusBus* bus) {
     LOG(WARNING) << "IBus connection is recovered.";
    MaybeRestoreConnections();
   }
 
  void IBusBusDisconnected(IBusBus* bus) {
     LOG(WARNING) << "IBus connection is terminated.";
    MaybeDestroyIBusConfig();
    VLOG(1) << "Notifying Chrome that IBus is terminated.";
    FOR_EACH_OBSERVER(Observer, observers_, OnConnectionChange(false));
   }
 
  void IBusBusGlobalEngineChanged(IBusBus* bus, const gchar* engine_name) {
     DCHECK(engine_name);
    VLOG(1) << "Global engine is changed to " << engine_name;
    UpdateUI(engine_name);
   }
 
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
 
  void FocusIn(IBusPanelService* panel, const gchar* input_context_path) {
    if (!input_context_path) {
      LOG(ERROR) << "NULL context passed";
    } else {
      VLOG(1) << "FocusIn: " << input_context_path;
    }
    // Remember the current ic path.
    input_context_path_ = Or(input_context_path, "");
   }
 
  void RegisterProperties(IBusPanelService* panel, IBusPropList* prop_list) {
    DoRegisterProperties(prop_list);
   }
 
  void UpdateProperty(IBusPanelService* panel, IBusProperty* ibus_prop) {
    VLOG(1) << "UpdateProperty";
    DCHECK(ibus_prop);

    // You can call
    //   LOG(INFO) << "\n" << PrintProp(ibus_prop, 0);
    // here to dump |ibus_prop|.

    ImePropertyList prop_list;  // our representation.
    if (!FlattenProperty(ibus_prop, &prop_list)) {
      // Don't update the UI on errors.
      LOG(ERROR) << "Malformed properties are detected";
      return;
    }
    // Notify the change.
    if (!prop_list.empty()) {
      FOR_EACH_OBSERVER(Observer, observers_,
                        OnUpdateImeProperty(prop_list));
    }
   }
 
  static void SetImeConfigCallback(GObject* source_object,
                                   GAsyncResult* res,
                                   gpointer user_data) {
    IBusConfig* config = IBUS_CONFIG(user_data);
    g_return_if_fail(config);

    GError* error = NULL;
    const gboolean result =
        ibus_config_set_value_async_finish(config, res, &error);

    if (!result) {
      std::string message = "(unknown error)";
      if (error && error->message) {
        message = error->message;
      }
      LOG(ERROR) << "ibus_config_set_value_async failed: " << message;
    }

    if (error) {
      g_error_free(error);
    }
     g_object_unref(config);
   }
 
   IBusBus* ibus_;
   IBusConfig* ibus_config_;
 
   std::string input_context_path_;

  ObserverList<Observer> observers_;
 };
