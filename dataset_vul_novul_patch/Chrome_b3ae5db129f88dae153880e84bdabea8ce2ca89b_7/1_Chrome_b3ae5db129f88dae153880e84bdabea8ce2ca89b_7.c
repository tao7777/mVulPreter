PowerLibrary* CrosLibrary::GetPowerLibrary() {
  return power_lib_.GetDefaultImpl(use_stub_impl_);
}
