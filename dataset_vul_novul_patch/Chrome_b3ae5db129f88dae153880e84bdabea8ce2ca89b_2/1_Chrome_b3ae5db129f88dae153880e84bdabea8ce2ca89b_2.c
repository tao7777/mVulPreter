InputMethodLibrary* CrosLibrary::GetInputMethodLibrary() {
  return input_method_lib_.GetDefaultImpl(use_stub_impl_);
}
