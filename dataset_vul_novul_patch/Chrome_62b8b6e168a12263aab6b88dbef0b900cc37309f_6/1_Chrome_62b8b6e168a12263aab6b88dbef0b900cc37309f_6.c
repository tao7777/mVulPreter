aura::Window* PartialMagnificationController::GetCurrentRootWindow() {
  aura::Window::Windows root_windows = Shell::GetAllRootWindows();
  for (aura::Window::Windows::const_iterator iter = root_windows.begin();
       iter != root_windows.end(); ++iter) {
    aura::Window* root_window = *iter;
    if (root_window->ContainsPointInRoot(
            root_window->GetHost()->dispatcher()->GetLastMouseLocationInRoot()))
      return root_window;
  }
  return NULL;
}
