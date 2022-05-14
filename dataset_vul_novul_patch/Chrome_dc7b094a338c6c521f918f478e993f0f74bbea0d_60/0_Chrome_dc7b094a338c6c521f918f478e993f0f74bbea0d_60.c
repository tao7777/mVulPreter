  static void IBusBusConnectedCallback(IBusBus* bus, gpointer user_data) {
  void IBusBusConnected(IBusBus* bus) {
     LOG(WARNING) << "IBus connection is recovered.";
    MaybeRestoreConnections();
   }
