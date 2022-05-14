   void ChangeCurrentInputMethodFromId(const std::string& input_method_id) {
    const chromeos::InputMethodDescriptor* descriptor =
        chromeos::input_method::GetInputMethodDescriptorFromId(
             input_method_id);
     if (descriptor) {
       ChangeCurrentInputMethod(*descriptor);
    } else {
      LOG(ERROR) << "Descriptor is not found for: " << input_method_id;
    }
   }
