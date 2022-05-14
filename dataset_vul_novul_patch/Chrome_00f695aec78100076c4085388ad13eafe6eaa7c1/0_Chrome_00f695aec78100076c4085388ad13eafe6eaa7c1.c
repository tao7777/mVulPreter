void DesktopNativeWidgetHelperAura::PreInitialize(
    aura::Window* window,
    const Widget::InitParams& params) {
#if !defined(OS_WIN)
  if (params.type == Widget::InitParams::TYPE_POPUP) {
    is_embedded_window_ = true;
    position_client_.reset(new EmbeddedWindowScreenPositionClient(widget_));
    aura::client::SetScreenPositionClient(window, position_client_.get());
    return;
  } else if (params.type == Widget::InitParams::TYPE_CONTROL) {
    return;
  }
#endif

  gfx::Rect bounds = params.bounds;
  if (bounds.IsEmpty()) {
    bounds.set_size(gfx::Size(100, 100));
  }
  root_window_.reset(new aura::RootWindow(bounds));
  root_window_->Init();
  root_window_->set_focus_manager(new aura::FocusManager);

  root_window_event_filter_ =
      new aura::shared::RootWindowEventFilter(root_window_.get());
   root_window_->SetEventFilter(root_window_event_filter_);
 
   input_method_filter_.reset(new aura::shared::InputMethodEventFilter());
  input_method_filter_->SetInputMethodPropertyInRootWindow(root_window_.get());
   root_window_event_filter_->AddFilter(input_method_filter_.get());
 
   aura::DesktopActivationClient* activation_client =
      new aura::DesktopActivationClient(root_window_.get());

#if defined(USE_X11)
  x11_window_event_filter_.reset(
      new X11WindowEventFilter(root_window_.get(), activation_client, widget_));
  x11_window_event_filter_->SetUseHostWindowBorders(false);
  root_window_event_filter_->AddFilter(x11_window_event_filter_.get());
#endif

  root_window_->AddRootWindowObserver(this);

  aura::client::SetActivationClient(root_window_.get(), activation_client);
  aura::client::SetDispatcherClient(root_window_.get(),
                                    new aura::DesktopDispatcherClient);

  position_client_.reset(
      new RootWindowScreenPositionClient(root_window_.get()));
  aura::client::SetScreenPositionClient(window, position_client_.get());
}
