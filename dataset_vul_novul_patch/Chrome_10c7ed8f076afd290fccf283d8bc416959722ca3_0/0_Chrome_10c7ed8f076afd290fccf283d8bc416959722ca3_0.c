void PanelBrowserView::OnWidgetActivationChanged(views::Widget* widget,
                                                 bool active) {
  ::BrowserView::OnWidgetActivationChanged(widget, active);

#if defined(OS_WIN) && !defined(USE_AURA)
  bool focused = active &&
      GetFrameView()->GetWidget()->GetNativeView() == ::GetForegroundWindow();
#else
  NOTIMPLEMENTED();
  bool focused = active;
#endif

  if (focused_ == focused)
     return;
   focused_ = focused;
 
  // Expand the panel if the minimized panel is activated by means other than
  // clicking on its titlebar. This is the workaround to support restoring the
  // minimized panel by other means, like alt-tabbing, win-tabbing, or clicking
  // the taskbar icon. Note that this workaround does not work for one edge
  // case: the mouse happens to be at the minimized panel when the user tries to
  // bring up the panel with the above alternatives.
  // When the user clicks on the minimized panel, the panel expansion will be
  // done when we process the mouse button pressed message.
  if (focused_ && panel_->IsMinimized() &&
      gfx::Screen::GetWindowAtCursorScreenPoint() !=
          widget->GetNativeWindow()) {
    panel_->Restore();
  }

   panel()->OnActiveStateChanged(focused);
 }
