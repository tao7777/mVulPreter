  virtual InputMethodDescriptor current_input_method() const {
     if (current_input_method_.id.empty()) {
       return input_method::GetFallbackInputMethodDescriptor();
     }
     return current_input_method_;
   }
