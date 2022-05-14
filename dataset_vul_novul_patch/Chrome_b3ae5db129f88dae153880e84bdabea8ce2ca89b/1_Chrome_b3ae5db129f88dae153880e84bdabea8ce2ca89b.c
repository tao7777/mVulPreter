BrightnessLibrary* CrosLibrary::GetBrightnessLibrary() {
  return brightness_lib_.GetDefaultImpl(use_stub_impl_);
}
