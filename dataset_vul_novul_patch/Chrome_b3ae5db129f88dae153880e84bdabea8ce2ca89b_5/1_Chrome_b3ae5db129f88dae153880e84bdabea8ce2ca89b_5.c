MountLibrary* CrosLibrary::GetMountLibrary() {
  return mount_lib_.GetDefaultImpl(use_stub_impl_);
}
