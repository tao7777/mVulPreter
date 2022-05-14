SystemLibrary* CrosLibrary::GetSystemLibrary() {
  return system_lib_.GetDefaultImpl(use_stub_impl_);
}
