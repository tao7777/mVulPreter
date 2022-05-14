  static void RegisterPropertiesCallback(IBusPanelService* panel,
                                         IBusPropList* prop_list,
                                         gpointer user_data) {
    g_return_if_fail(user_data);
    InputMethodStatusConnection* self
        = static_cast<InputMethodStatusConnection*>(user_data);
    self->RegisterProperties(prop_list);
   }
