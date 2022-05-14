   void MaybeStartInputMethodDaemon(const std::string& section,
                                    const std::string& config_name,
                                   const input_method::ImeConfigValue& value) {
     if (section == language_prefs::kGeneralSectionName &&
         config_name == language_prefs::kPreloadEnginesConfigName &&
        value.type == input_method::ImeConfigValue::kValueTypeStringList &&
         !value.string_list_value.empty()) {
      if (ContainOnlyOneKeyboardLayout(value) || defer_ime_startup_) {
        return;
      }

      const bool just_started = StartInputMethodDaemon();
      if (!just_started) {
        return;
      }

      if (tentative_current_input_method_id_.empty()) {
        tentative_current_input_method_id_ = current_input_method_.id;
      }

      if (std::find(value.string_list_value.begin(),
                    value.string_list_value.end(),
                    tentative_current_input_method_id_)
          == value.string_list_value.end()) {
        tentative_current_input_method_id_.clear();
      }
    }
  }
