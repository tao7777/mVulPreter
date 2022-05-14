 bool CustomButton::AcceleratorPressed(const ui::Accelerator& accelerator) {
  // Should only handle accelerators when active. However, only top level
  // widgets can be active, so for child widgets check if they are focused
  // instead.
  if ((IsChildWidget() && !FocusInChildWidget()) ||
      (!IsChildWidget() && !GetWidget()->IsActive())) {
    return false;
  }
   SetState(STATE_NORMAL);
   ui::MouseEvent synthetic_event(
      ui::ET_MOUSE_RELEASED, gfx::Point(), gfx::Point(), ui::EventTimeForNow(),
      ui::EF_LEFT_MOUSE_BUTTON, ui::EF_LEFT_MOUSE_BUTTON);
  NotifyClick(synthetic_event);
  return true;
}
