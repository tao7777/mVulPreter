 void DesktopWindowTreeHostX11::SetVisible(bool visible) {
  if (is_compositor_set_visible_ == visible)
    return;

  is_compositor_set_visible_ = visible;
   if (compositor())
     compositor()->SetVisible(visible);
  native_widget_delegate_->OnNativeWidgetVisibilityChanged(visible);
 }
