KeyboardLibrary* CrosLibrary::GetKeyboardLibrary() {
  return keyboard_lib_.GetDefaultImpl(use_stub_impl_);
}
