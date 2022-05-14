void PartialMagnificationController::CreateMagnifierWindow() {
  if (zoom_widget_)
     return;
 
  aura::Window* root_window = GetCurrentRootWindow();
  if (!root_window)
     return;
 
   root_window->AddObserver(this);
 
   gfx::Point mouse(
       root_window->GetHost()->dispatcher()->GetLastMouseLocationInRoot());
 
  zoom_widget_ = new views::Widget;
   views::Widget::InitParams params(
       views::Widget::InitParams::TYPE_WINDOW_FRAMELESS);
   params.activatable = views::Widget::InitParams::ACTIVATABLE_NO;
   params.accept_events = false;
   params.opacity = views::Widget::InitParams::TRANSLUCENT_WINDOW;
   params.parent = root_window;
  zoom_widget_->Init(params);
  zoom_widget_->SetBounds(gfx::Rect(mouse.x() - kMagnifierWidth / 2,
                                    mouse.y() - kMagnifierHeight / 2,
                                    kMagnifierWidth, kMagnifierHeight));
  zoom_widget_->set_focus_on_creation(false);
  zoom_widget_->Show();
  aura::Window* window = zoom_widget_->GetNativeView();
   window->SetName(kPartialMagniferWindowName);
 
  zoom_widget_->GetNativeView()->layer()->SetBounds(
      gfx::Rect(0, 0, kMagnifierWidth, kMagnifierHeight));
  zoom_widget_->GetNativeView()->layer()->SetBackgroundZoom(scale_, kZoomInset);
 
  zoom_widget_->AddObserver(this);
 }
