  static void IBusBusGlobalEngineChangedCallback(
      IBusBus* bus, const gchar* engine_name, gpointer user_data) {
     DCHECK(engine_name);
    DLOG(INFO) << "Global engine is changed to " << engine_name;
    g_return_if_fail(user_data);
    InputMethodStatusConnection* self
        = static_cast<InputMethodStatusConnection*>(user_data);
    self->UpdateUI(engine_name);
   }
