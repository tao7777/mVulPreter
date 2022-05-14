  ReleaseAccelerator(ui::KeyboardCode keycode,
  ReleaseAccelerator(ui::KeyboardCode keycode, int modifiers)
      : ui::Accelerator(keycode, modifiers) {
     set_type(ui::ET_KEY_RELEASED);
   }
