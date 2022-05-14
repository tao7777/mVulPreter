CryptohomeLibrary* CrosLibrary::GetCryptohomeLibrary() {
  return crypto_lib_.GetDefaultImpl(use_stub_impl_);
}
