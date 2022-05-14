void CrosLibrary::TestApi::SetPowerLibrary(
    PowerLibrary* library, bool own) {
  library_->power_lib_.SetImpl(library, own);
}
