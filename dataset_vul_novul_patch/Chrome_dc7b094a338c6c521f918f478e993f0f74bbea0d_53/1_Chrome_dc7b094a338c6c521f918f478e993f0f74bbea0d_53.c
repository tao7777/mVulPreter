  void ConnectIBusSignals() {
    if (!ibus_) {
      return;
    }

     g_signal_connect_after(ibus_,
                            "connected",
                           G_CALLBACK(IBusBusConnectedCallback),
                            this);
 
     g_signal_connect(ibus_,
                      "disconnected",
                     G_CALLBACK(IBusBusDisconnectedCallback),
                      this);
     g_signal_connect(ibus_,
                      "global-engine-changed",
                     G_CALLBACK(IBusBusGlobalEngineChangedCallback),
                      this);
     g_signal_connect(ibus_,
                      "name-owner-changed",
                     G_CALLBACK(IBusBusNameOwnerChangedCallback),
                      this);
   }
