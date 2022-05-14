BurnLibrary* CrosLibrary::GetBurnLibrary() {
  return burn_lib_.GetDefaultImpl(use_stub_impl_);
}
