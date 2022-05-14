void ScreenPositionController::ConvertHostPointToRelativeToRootWindow(
    aura::Window* root_window,
    const aura::Window::Windows& root_windows,
    gfx::Point* point,
    aura::Window** target_root) {
  DCHECK(!root_window->parent());
   gfx::Point point_in_root(*point);
   root_window->GetHost()->ConvertPointFromHost(&point_in_root);
 
 #if defined(USE_X11) || defined(USE_OZONE)
  gfx::Rect host_bounds(root_window->GetHost()->GetBounds().size());
  if (!host_bounds.Contains(*point)) {

    gfx::Point location_in_native(point_in_root);

    root_window->GetHost()->ConvertPointToNativeScreen(&location_in_native);

    for (size_t i = 0; i < root_windows.size(); ++i) {
      aura::WindowTreeHost* host = root_windows[i]->GetHost();
      const gfx::Rect native_bounds = host->GetBounds();
      if (native_bounds.Contains(location_in_native)) {
         *target_root = root_windows[i];
         *point = location_in_native;
         host->ConvertPointFromNativeScreen(point);
        return;
       }
     }
   }
 #endif
  *target_root = root_window;
  *point = point_in_root;
 }
