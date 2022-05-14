void CrosLibrary::TestApi::SetMountLibrary(
    MountLibrary* library, bool own) {
  library_->mount_lib_.SetImpl(library, own);
}
