 void DesktopWindowTreeHostX11::SetVisible(bool visible) {
   if (compositor())
     compositor()->SetVisible(visible);
  if (IsVisible() != visible)
    native_widget_delegate_->OnNativeWidgetVisibilityChanged(visible);
 }
