  static void IBusBusDisconnectedCallback(IBusBus* bus, gpointer user_data) {
     LOG(WARNING) << "IBus connection is terminated.";
    g_return_if_fail(user_data);
    InputMethodStatusConnection* self
        = static_cast<InputMethodStatusConnection*>(user_data);
    self->MaybeDestroyIBusConfig();
    if (self->connection_change_handler_) {
      LOG(INFO) << "Notifying Chrome that IBus is terminated.";
      self->connection_change_handler_(self->language_library_, false);
    }
   }
