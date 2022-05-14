void RenderWidgetHostViewAura::WasHidden() {
  if (host_->is_hidden())
    return;
  host_->WasHidden();
 
   released_front_lock_ = NULL;
 
 #if defined(OS_WIN)
   aura::RootWindow* root_window = window_->GetRootWindow();
   if (root_window) {
    HWND parent = root_window->GetAcceleratedWidget();
    LPARAM lparam = reinterpret_cast<LPARAM>(this);

    EnumChildWindows(parent, HideWindowsCallback, lparam);
  }
#endif
}
