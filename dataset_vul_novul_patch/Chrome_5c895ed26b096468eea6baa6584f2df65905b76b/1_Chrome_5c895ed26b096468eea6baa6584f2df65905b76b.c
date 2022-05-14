 bool PasswordAutofillAgent::TryToShowTouchToFill(
     const WebFormControlElement& control_element) {
   const WebInputElement* element = ToWebInputElement(&control_element);
  if (!element || (!base::Contains(web_input_to_password_info_, *element) &&
                   !base::Contains(password_to_username_, *element))) {
     return false;
   }
   if (was_touch_to_fill_ui_shown_)
    return false;
  was_touch_to_fill_ui_shown_ = true;

  GetPasswordManagerDriver()->ShowTouchToFill();
  return true;
}
