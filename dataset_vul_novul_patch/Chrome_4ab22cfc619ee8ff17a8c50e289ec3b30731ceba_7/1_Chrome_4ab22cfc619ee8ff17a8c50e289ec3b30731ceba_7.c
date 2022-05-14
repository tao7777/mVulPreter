bool KeyMap::Press(const scoped_refptr<WindowProxy>& window,
                   const ui::KeyboardCode key_code,
                   const wchar_t& key) {
  if (key_code == ui::VKEY_SHIFT) {
    shift_ = !shift_;
  } else if (key_code == ui::VKEY_CONTROL) {
    control_ = !control_;
  } else if (key_code == ui::VKEY_MENU) {  // ALT
    alt_ = !alt_;
  } else if (key_code == ui::VKEY_COMMAND) {
    command_ = !command_;
  }

  int modifiers = 0;
  if (shift_ || shifted_keys_.find(key) != shifted_keys_.end()) {
    modifiers = modifiers | ui::EF_SHIFT_DOWN;
  }
  if (control_) {
    modifiers = modifiers | ui::EF_CONTROL_DOWN;
  }
  if (alt_) {
     modifiers = modifiers | ui::EF_ALT_DOWN;
   }
   if (command_) {
    VLOG(1) << "Pressing command key on linux!!";
     modifiers = modifiers | ui::EF_COMMAND_DOWN;
   }
 
  window->SimulateOSKeyPress(key_code, modifiers);

  return true;
}
