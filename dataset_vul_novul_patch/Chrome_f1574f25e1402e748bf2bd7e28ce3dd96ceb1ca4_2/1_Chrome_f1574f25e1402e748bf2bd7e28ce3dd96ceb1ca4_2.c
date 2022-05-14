 void HTMLInputElement::HandleBlurEvent() {
  input_type_->DisableSecureTextInput();
   input_type_view_->HandleBlurEvent();
 }
