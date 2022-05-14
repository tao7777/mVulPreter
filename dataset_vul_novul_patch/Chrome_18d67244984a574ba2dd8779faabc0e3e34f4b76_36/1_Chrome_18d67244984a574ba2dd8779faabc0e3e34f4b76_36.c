void RenderWidgetHostViewAura::WasShown() {
  if (!host_->is_hidden())
    return;
  host_->WasShown();

  if (!current_surface_ && host_->is_accelerated_compositing_active() &&
      !released_front_lock_.get()) {
     released_front_lock_ = GetCompositor()->GetCompositorLock();
   }
 
  AdjustSurfaceProtection();
 #if defined(OS_WIN)
   LPARAM lparam = reinterpret_cast<LPARAM>(this);
   EnumChildWindows(ui::GetHiddenWindow(), ShowWindowsCallback, lparam);
#endif
}
