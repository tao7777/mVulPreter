void CrosLibrary::TestApi::SetBurnLibrary(
    BurnLibrary* library, bool own) {
  library_->burn_lib_.SetImpl(library, own);
}
