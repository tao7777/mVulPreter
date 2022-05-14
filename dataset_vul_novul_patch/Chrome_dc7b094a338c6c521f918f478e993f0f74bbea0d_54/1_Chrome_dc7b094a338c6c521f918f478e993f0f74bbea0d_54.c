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
                     G_CALLBACK(FocusInCallback),
                      this);
     g_signal_connect(ibus_panel_service,
                      "register-properties",
                     G_CALLBACK(RegisterPropertiesCallback),
                      this);
     g_signal_connect(ibus_panel_service,
                      "update-property",
                     G_CALLBACK(UpdatePropertyCallback),
                      this);
   }
