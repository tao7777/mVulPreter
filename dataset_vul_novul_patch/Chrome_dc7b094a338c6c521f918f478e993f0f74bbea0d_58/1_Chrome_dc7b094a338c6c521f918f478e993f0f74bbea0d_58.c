  static InputMethodStatusConnection* GetConnection(
      void* language_library,
      LanguageCurrentInputMethodMonitorFunction current_input_method_changed,
      LanguageRegisterImePropertiesFunction register_ime_properties,
      LanguageUpdateImePropertyFunction update_ime_property,
      LanguageConnectionChangeMonitorFunction connection_change_handler) {
    DCHECK(language_library);
    DCHECK(current_input_method_changed),
    DCHECK(register_ime_properties);
    DCHECK(update_ime_property);
    InputMethodStatusConnection* object = GetInstance();
    if (!object->language_library_) {
      object->language_library_ = language_library;
      object->current_input_method_changed_ = current_input_method_changed;
      object->register_ime_properties_= register_ime_properties;
      object->update_ime_property_ = update_ime_property;
      object->connection_change_handler_ = connection_change_handler;
      object->MaybeRestoreConnections();
    } else if (object->language_library_ != language_library) {
      LOG(ERROR) << "Unknown language_library is passed";
    }
    return object;
   }
