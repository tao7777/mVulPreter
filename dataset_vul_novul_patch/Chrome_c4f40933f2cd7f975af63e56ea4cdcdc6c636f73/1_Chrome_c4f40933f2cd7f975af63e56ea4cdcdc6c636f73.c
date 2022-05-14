  ReleaseAccelerator(ui::KeyboardCode keycode,
                     bool shift_pressed,
                     bool ctrl_pressed,
                     bool alt_pressed)
      : ui::Accelerator(keycode, shift_pressed, ctrl_pressed, alt_pressed) {
     set_type(ui::ET_KEY_RELEASED);
   }
