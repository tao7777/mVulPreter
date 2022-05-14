SyslogsLibrary* CrosLibrary::GetSyslogsLibrary() {
  return syslogs_lib_.GetDefaultImpl(use_stub_impl_);
}
