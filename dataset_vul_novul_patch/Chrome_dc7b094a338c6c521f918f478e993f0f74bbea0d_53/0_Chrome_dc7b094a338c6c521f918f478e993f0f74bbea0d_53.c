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
