  InputMethodStatusConnection()
      : current_input_method_changed_(NULL),
        register_ime_properties_(NULL),
        update_ime_property_(NULL),
        connection_change_handler_(NULL),
        language_library_(NULL),
        ibus_(NULL),
         ibus_config_(NULL) {
   }
