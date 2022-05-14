void TabletModeWindowState::LeaveTabletMode(wm::WindowState* window_state) {
void TabletModeWindowState::LeaveTabletMode(wm::WindowState* window_state,
                                            bool was_in_overview) {
  // Only do bounds change animation if the window was showing in overview,
  // or the top window or a window showing in splitview before leaving tablet
  // mode, and the window has changed its state. Otherwise, restore its bounds
  // immediately.
   EnterAnimationType animation_type =
      was_in_overview || window_state->IsSnapped() ||
              IsTopWindow(window_state->window())
           ? DEFAULT
           : IMMEDIATE;
   if (old_state_->GetType() == window_state->GetStateType() &&
      !window_state->IsNormalStateType()) {
    animation_type = IMMEDIATE;
  }
  old_state_->set_enter_animation_type(animation_type);
  std::unique_ptr<wm::WindowState::State> our_reference =
      window_state->SetStateObject(std::move(old_state_));
}
