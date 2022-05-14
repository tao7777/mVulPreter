void CrosLibrary::TestApi::SetInputMethodLibrary(
    InputMethodLibrary* library,  bool own) {
  library_->input_method_lib_.SetImpl(library, own);
}
