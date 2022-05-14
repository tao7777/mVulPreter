  static void IBusBusDisconnectedCallback(IBusBus* bus, gpointer user_data) {
  void IBusBusDisconnected(IBusBus* bus) {
     LOG(WARNING) << "IBus connection is terminated.";
    MaybeDestroyIBusConfig();
    VLOG(1) << "Notifying Chrome that IBus is terminated.";
    FOR_EACH_OBSERVER(Observer, observers_, OnConnectionChange(false));
   }
