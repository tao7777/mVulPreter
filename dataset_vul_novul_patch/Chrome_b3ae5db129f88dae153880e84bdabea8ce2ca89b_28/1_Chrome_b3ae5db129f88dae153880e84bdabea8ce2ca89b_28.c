void CrosLibrary::TestApi::SetUpdateLibrary(
    UpdateLibrary* library, bool own) {
  library_->update_lib_.SetImpl(library, own);
}
