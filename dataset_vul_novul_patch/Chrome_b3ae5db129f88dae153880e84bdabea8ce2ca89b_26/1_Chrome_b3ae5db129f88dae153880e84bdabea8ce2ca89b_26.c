void CrosLibrary::TestApi::SetSystemLibrary(
    SystemLibrary* library, bool own) {
  library_->system_lib_.SetImpl(library, own);
}
