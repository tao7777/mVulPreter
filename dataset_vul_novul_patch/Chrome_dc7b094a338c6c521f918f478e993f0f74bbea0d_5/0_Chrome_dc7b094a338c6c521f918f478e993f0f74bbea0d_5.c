  bool ChangeInputMethodViaIBus(const std::string& input_method_id) {
    if (!initialized_successfully_)
      return false;

    std::string input_method_id_to_switch = input_method_id;

    if (!InputMethodIsActivated(input_method_id)) {
      scoped_ptr<input_method::InputMethodDescriptors> input_methods(
          GetActiveInputMethods());
       DCHECK(!input_methods->empty());
       if (!input_methods->empty()) {
         input_method_id_to_switch = input_methods->at(0).id;
        LOG(INFO) << "Can't change the current input method to "
                  << input_method_id << " since the engine is not preloaded. "
                  << "Switch to " << input_method_id_to_switch << " instead.";
       }
     }
 
    if (ibus_controller_->ChangeInputMethod(input_method_id_to_switch)) {
       return true;
     }
 
    LOG(ERROR) << "Can't switch input method to " << input_method_id_to_switch;
    return false;
  }
