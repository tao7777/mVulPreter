void HTMLInputElement::HandleFocusEvent(Element* old_focused_element,
                                        WebFocusType type) {
  input_type_->EnableSecureTextInput();
}
