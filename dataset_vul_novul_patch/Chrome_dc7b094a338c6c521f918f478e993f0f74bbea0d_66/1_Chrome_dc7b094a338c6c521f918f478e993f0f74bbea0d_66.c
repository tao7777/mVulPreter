  void MaybeRestoreConnections() {
    if (IBusConnectionsAreAlive()) {
      return;
    }
    MaybeCreateIBus();
     MaybeRestoreIBusConfig();
     if (IBusConnectionsAreAlive()) {
       ConnectPanelServiceSignals();
      if (connection_change_handler_) {
        LOG(INFO) << "Notifying Chrome that IBus is ready.";
        connection_change_handler_(language_library_, true);
      }
     }
   }
