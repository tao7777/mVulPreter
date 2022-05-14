   virtual bool SetImeConfig(const std::string& section,
                             const std::string& config_name,
                            const ImeConfigValue& value) {
     if (section == language_prefs::kGeneralSectionName &&
         config_name == language_prefs::kPreloadEnginesConfigName &&
        value.type == ImeConfigValue::kValueTypeStringList) {
       active_input_method_ids_ = value.string_list_value;
     }
 
    MaybeStartInputMethodDaemon(section, config_name, value);

    const ConfigKeyType key = std::make_pair(section, config_name);
    current_config_values_[key] = value;
    if (ime_connected_) {
      pending_config_requests_[key] = value;
      FlushImeConfig();
    }

    MaybeStopInputMethodDaemon(section, config_name, value);
    MaybeChangeCurrentKeyboardLayout(section, config_name, value);
     return pending_config_requests_.empty();
   }
