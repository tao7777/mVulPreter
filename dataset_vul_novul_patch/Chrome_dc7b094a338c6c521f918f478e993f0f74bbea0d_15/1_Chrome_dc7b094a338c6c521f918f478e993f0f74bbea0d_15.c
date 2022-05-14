   virtual size_t GetNumActiveInputMethods() {
    scoped_ptr<InputMethodDescriptors> descriptors(GetActiveInputMethods());
     return descriptors->size();
   }
