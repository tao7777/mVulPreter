   virtual size_t GetNumActiveInputMethods() {
    scoped_ptr<input_method::InputMethodDescriptors> input_methods(
        GetActiveInputMethods());
     return input_methods->size();
   }
