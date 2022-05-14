InputMethodDescriptors* CrosMock::CreateInputMethodDescriptors() {
input_method::InputMethodDescriptors*
CrosMock::CreateInputMethodDescriptors() {
  input_method::InputMethodDescriptors* descriptors =
      new input_method::InputMethodDescriptors;
   descriptors->push_back(
       input_method::GetFallbackInputMethodDescriptor());
   return descriptors;
}
