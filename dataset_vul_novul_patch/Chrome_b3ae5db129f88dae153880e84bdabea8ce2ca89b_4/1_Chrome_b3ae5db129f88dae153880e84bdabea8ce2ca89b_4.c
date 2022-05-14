LoginLibrary* CrosLibrary::GetLoginLibrary() {
  return login_lib_.GetDefaultImpl(use_stub_impl_);
}
