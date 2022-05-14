  void FlushImeConfig() {
    if (!initialized_successfully_)
      return;

    bool active_input_methods_are_changed = false;
    InputMethodConfigRequests::iterator iter =
        pending_config_requests_.begin();
     while (iter != pending_config_requests_.end()) {
       const std::string& section = iter->first.first;
       const std::string& config_name = iter->first.second;
      input_method::ImeConfigValue& value = iter->second;
 
       if (config_name == language_prefs::kPreloadEnginesConfigName &&
           !tentative_current_input_method_id_.empty()) {
        std::vector<std::string>::iterator engine_iter = std::find(
            value.string_list_value.begin(),
            value.string_list_value.end(),
            tentative_current_input_method_id_);
        if (engine_iter != value.string_list_value.end()) {
          std::rotate(value.string_list_value.begin(),
                      engine_iter,  // this becomes the new first element
                      value.string_list_value.end());
        } else {
          LOG(WARNING) << tentative_current_input_method_id_
                       << " is not in preload_engines: " << value.ToString();
        }
         tentative_current_input_method_id_.erase();
       }
 
      if (ibus_controller_->SetImeConfig(section, config_name, value)) {
         if (config_name == language_prefs::kPreloadEnginesConfigName) {
           active_input_methods_are_changed = true;
          VLOG(1) << "Updated preload_engines: " << value.ToString();
        }
        pending_config_requests_.erase(iter++);
      } else {
        break;
      }
    }

    if (active_input_methods_are_changed) {
       const size_t num_active_input_methods = GetNumActiveInputMethods();
      FOR_EACH_OBSERVER(InputMethodLibrary::Observer, observers_,
                         ActiveInputMethodsChanged(this,
                                                   current_input_method_,
                                                   num_active_input_methods));
     }
   }
