  void ChangeCurrentInputMethod(const InputMethodDescriptor& new_input_method) {
  void ChangeCurrentInputMethod(const input_method::InputMethodDescriptor&
                                new_input_method) {
     if (current_input_method_.id != new_input_method.id) {
       previous_input_method_ = current_input_method_;
       current_input_method_ = new_input_method;

      if (!input_method::SetCurrentKeyboardLayoutByName(
              current_input_method_.keyboard_layout)) {
        LOG(ERROR) << "Failed to change keyboard layout to "
                   << current_input_method_.keyboard_layout;
      }

      ObserverListBase<InputMethodLibrary::Observer>::Iterator it(observers_);
      InputMethodLibrary::Observer* first_observer = it.GetNext();
       if (first_observer) {
         first_observer->PreferenceUpdateNeeded(this,
                                                previous_input_method_,
                                               current_input_method_);
      }
    }

     const size_t num_active_input_methods = GetNumActiveInputMethods();
    FOR_EACH_OBSERVER(InputMethodLibrary::Observer, observers_,
                       InputMethodChanged(this,
                                          current_input_method_,
                                          num_active_input_methods));
  }
