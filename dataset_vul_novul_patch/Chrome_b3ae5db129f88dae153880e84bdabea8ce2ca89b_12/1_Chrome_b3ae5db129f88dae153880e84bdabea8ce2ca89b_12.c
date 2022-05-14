TouchpadLibrary* CrosLibrary::GetTouchpadLibrary() {
  return touchpad_lib_.GetDefaultImpl(use_stub_impl_);
}
