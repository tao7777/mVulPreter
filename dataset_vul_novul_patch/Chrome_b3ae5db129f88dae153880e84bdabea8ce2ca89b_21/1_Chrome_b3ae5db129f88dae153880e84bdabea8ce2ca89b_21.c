void CrosLibrary::TestApi::SetNetworkLibrary(
    NetworkLibrary* library, bool own) {
  library_->network_lib_.SetImpl(library, own);
}
