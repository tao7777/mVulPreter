void CrosLibrary::TestApi::SetCryptohomeLibrary(
    CryptohomeLibrary* library, bool own) {
  library_->crypto_lib_.SetImpl(library, own);
}
