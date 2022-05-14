void CrosLibrary::TestApi::SetLoginLibrary(
    LoginLibrary* library, bool own) {
  library_->login_lib_.SetImpl(library, own);
}
