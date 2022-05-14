InputMethodDescriptors* CrosMock::CreateInputMethodDescriptors() {
  InputMethodDescriptors* descriptors = new InputMethodDescriptors;
   descriptors->push_back(
       input_method::GetFallbackInputMethodDescriptor());
   return descriptors;
}
