  static void FocusInCallback(IBusPanelService* panel,
                              const gchar* path,
                              gpointer user_data) {
    g_return_if_fail(user_data);
    InputMethodStatusConnection* self
        = static_cast<InputMethodStatusConnection*>(user_data);
    self->FocusIn(path);
   }
