  virtual InputMethodDescriptors* GetSupportedInputMethods() {
  virtual input_method::InputMethodDescriptors* GetSupportedInputMethods() {
     if (!initialized_successfully_) {
      input_method::InputMethodDescriptors* result =
          new input_method::InputMethodDescriptors;
       result->push_back(input_method::GetFallbackInputMethodDescriptor());
       return result;
     }
 
    return input_method::GetSupportedInputMethodDescriptors();
   }
