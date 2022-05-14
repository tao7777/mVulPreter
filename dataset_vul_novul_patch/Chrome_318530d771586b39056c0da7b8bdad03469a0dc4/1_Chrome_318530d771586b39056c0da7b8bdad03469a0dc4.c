bool IsSmartVirtualKeyboardEnabled() {
  if (base::CommandLine::ForCurrentProcess()->HasSwitch(
           keyboard::switches::kEnableVirtualKeyboard)) {
     return false;
   }
  return !base::CommandLine::ForCurrentProcess()->HasSwitch(
      keyboard::switches::kDisableSmartVirtualKeyboard);
 }
