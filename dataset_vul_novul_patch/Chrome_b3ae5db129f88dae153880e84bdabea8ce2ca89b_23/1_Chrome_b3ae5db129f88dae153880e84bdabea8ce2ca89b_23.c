void CrosLibrary::TestApi::SetScreenLockLibrary(
    ScreenLockLibrary* library, bool own) {
  library_->screen_lock_lib_.SetImpl(library, own);
}
