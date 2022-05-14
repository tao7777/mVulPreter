void CrosLibrary::TestApi::SetKeyboardLibrary(
    KeyboardLibrary* library, bool own) {
  library_->keyboard_lib_.SetImpl(library, own);
}
