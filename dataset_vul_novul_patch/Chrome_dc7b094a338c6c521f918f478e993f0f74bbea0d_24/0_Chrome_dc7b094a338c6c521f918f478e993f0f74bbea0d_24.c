   void MaybeStopInputMethodDaemon(const std::string& section,
                                   const std::string& config_name,
                                  const input_method::ImeConfigValue& value) {
    if (section == language_prefs::kGeneralSectionName &&
        config_name == language_prefs::kPreloadEnginesConfigName &&
        ContainOnlyOneKeyboardLayout(value) &&
        enable_auto_ime_shutdown_) {
      StopInputMethodDaemon();
    }
  }
