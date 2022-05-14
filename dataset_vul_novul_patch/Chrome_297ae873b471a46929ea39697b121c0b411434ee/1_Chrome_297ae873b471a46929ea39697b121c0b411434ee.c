 bool CustomButton::AcceleratorPressed(const ui::Accelerator& accelerator) {
   SetState(STATE_NORMAL);
   ui::MouseEvent synthetic_event(
      ui::ET_MOUSE_RELEASED, gfx::Point(), gfx::Point(), ui::EventTimeForNow(),
      ui::EF_LEFT_MOUSE_BUTTON, ui::EF_LEFT_MOUSE_BUTTON);
  NotifyClick(synthetic_event);
  return true;
}
