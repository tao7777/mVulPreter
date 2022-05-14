  virtual InputMethodDescriptors* GetSupportedInputMethods() {
     if (!initialized_successfully_) {
      InputMethodDescriptors* result = new InputMethodDescriptors;
       result->push_back(input_method::GetFallbackInputMethodDescriptor());
       return result;
     }
 
    return chromeos::GetSupportedInputMethodDescriptors();
   }
