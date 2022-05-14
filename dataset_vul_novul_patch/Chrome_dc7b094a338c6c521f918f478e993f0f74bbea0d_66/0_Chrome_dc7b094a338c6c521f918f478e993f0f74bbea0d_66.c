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
