  void UpdateUI(const char* current_global_engine_id) {
    DCHECK(current_global_engine_id);

    const IBusEngineInfo* engine_info = NULL;
    for (size_t i = 0; i < arraysize(kIBusEngines); ++i) {
      if (kIBusEngines[i].name == std::string(current_global_engine_id)) {
        engine_info = &kIBusEngines[i];
        break;
      }
    }

    if (!engine_info) {
      LOG(ERROR) << current_global_engine_id
                 << " is not found in the input method white-list.";
      return;
    }

    InputMethodDescriptor current_input_method =
        CreateInputMethodDescriptor(engine_info->name,
                                    engine_info->longname,
                                     engine_info->layout,
                                     engine_info->language);
 
    VLOG(1) << "Updating the UI. ID:" << current_input_method.id
            << ", keyboard_layout:" << current_input_method.keyboard_layout;
 
    FOR_EACH_OBSERVER(Observer, observers_,
                      OnCurrentInputMethodChanged(current_input_method));
   }
