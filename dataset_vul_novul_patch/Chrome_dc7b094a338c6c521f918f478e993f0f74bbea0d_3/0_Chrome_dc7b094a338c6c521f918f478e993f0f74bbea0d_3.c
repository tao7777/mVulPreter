   void ChangeCurrentInputMethodFromId(const std::string& input_method_id) {
    const input_method::InputMethodDescriptor* descriptor =
        input_method::GetInputMethodDescriptorFromId(
             input_method_id);
     if (descriptor) {
       ChangeCurrentInputMethod(*descriptor);
    } else {
      LOG(ERROR) << "Descriptor is not found for: " << input_method_id;
    }
   }
