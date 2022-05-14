void CrosLibrary::TestApi::SetTouchpadLibrary(
    TouchpadLibrary* library, bool own) {
  library_->touchpad_lib_.SetImpl(library, own);
}
