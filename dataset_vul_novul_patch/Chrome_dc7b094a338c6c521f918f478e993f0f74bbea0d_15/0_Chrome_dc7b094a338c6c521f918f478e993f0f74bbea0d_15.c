   virtual size_t GetNumActiveInputMethods() {
    scoped_ptr<input_method::InputMethodDescriptors> descriptors(
        GetActiveInputMethods());
     return descriptors->size();
   }
