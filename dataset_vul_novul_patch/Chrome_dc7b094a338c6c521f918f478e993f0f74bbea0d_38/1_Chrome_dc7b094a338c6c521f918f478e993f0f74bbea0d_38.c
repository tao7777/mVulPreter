  virtual InputMethodDescriptor previous_input_method() const {
     if (previous_input_method_.id.empty()) {
       return input_method::GetFallbackInputMethodDescriptor();
     }
     return previous_input_method_;
   }
