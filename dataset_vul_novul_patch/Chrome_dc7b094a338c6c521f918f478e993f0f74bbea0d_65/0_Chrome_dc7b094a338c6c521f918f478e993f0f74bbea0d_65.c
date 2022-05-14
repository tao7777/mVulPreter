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
