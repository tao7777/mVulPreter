   virtual size_t GetNumActiveInputMethods() {
    scoped_ptr<InputMethodDescriptors> input_methods(GetActiveInputMethods());
     return input_methods->size();
   }
