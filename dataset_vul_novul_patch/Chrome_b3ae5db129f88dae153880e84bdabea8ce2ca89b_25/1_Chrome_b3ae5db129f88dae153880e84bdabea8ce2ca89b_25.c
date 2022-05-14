void CrosLibrary::TestApi::SetSyslogsLibrary(
    SyslogsLibrary* library, bool own) {
  library_->syslogs_lib_.SetImpl(library, own);
}
