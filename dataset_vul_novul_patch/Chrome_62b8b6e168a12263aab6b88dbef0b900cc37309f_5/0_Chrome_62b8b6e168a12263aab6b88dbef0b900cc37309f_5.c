void PartialMagnificationController::CreateMagnifierWindow() {
  if (event->type() == ui::ET_MOUSE_PRESSED)
    SetActive(true);

  if (event->type() == ui::ET_MOUSE_RELEASED)
    SetActive(false);

  if (!is_active_)
     return;
 
  // If the previous root window was detached host_widget_ will be null;
  // reconstruct it. We also need to change the root window if the cursor has
  // crossed display boundries.
  SwitchTargetRootWindowIfNeeded(GetCurrentRootWindow());

  // If that failed for any reason return.
  if (!host_widget_) {
    SetActive(false);
    return;
  }

  gfx::Point point = event->root_location();

  // Remap point from where it was captured to the display it is actually on.
  aura::Window* target = static_cast<aura::Window*>(event->target());
  aura::Window* event_root = target->GetRootWindow();
  aura::Window::ConvertPointToTarget(
      event_root, host_widget_->GetNativeView()->GetRootWindow(), &point);

  host_widget_->SetBounds(GetBounds(point));

  event->StopPropagation();
}

void PartialMagnificationController::CreateMagnifierWindow(
    aura::Window* root_window) {
  if (host_widget_ || !root_window)
     return;
 
   root_window->AddObserver(this);
 
   gfx::Point mouse(
       root_window->GetHost()->dispatcher()->GetLastMouseLocationInRoot());
 
  host_widget_ = new views::Widget;
   views::Widget::InitParams params(
       views::Widget::InitParams::TYPE_WINDOW_FRAMELESS);
   params.activatable = views::Widget::InitParams::ACTIVATABLE_NO;
   params.accept_events = false;
  params.bounds = GetBounds(mouse);
   params.opacity = views::Widget::InitParams::TRANSLUCENT_WINDOW;
   params.parent = root_window;
  host_widget_->Init(params);
  host_widget_->set_focus_on_creation(false);
  host_widget_->Show();

  aura::Window* window = host_widget_->GetNativeView();
   window->SetName(kPartialMagniferWindowName);
 
  ui::Layer* root_layer = host_widget_->GetNativeView()->layer();

  zoom_layer_.reset(new ui::Layer(ui::LayerType::LAYER_SOLID_COLOR));
  zoom_layer_->SetBounds(gfx::Rect(GetWindowSize()));
  zoom_layer_->SetBackgroundZoom(kMagnificationScale, kZoomInset);
  root_layer->Add(zoom_layer_.get());

  border_layer_.reset(new ui::Layer(ui::LayerType::LAYER_SOLID_COLOR));
  border_layer_->SetBounds(gfx::Rect(GetWindowSize()));
  border_layer_->SetColor(SK_ColorWHITE);
  root_layer->Add(border_layer_.get());

  border_mask_.reset(new ContentMask(true, GetWindowSize()));
  border_layer_->SetMaskLayer(border_mask_->layer());
 
  zoom_mask_.reset(new ContentMask(false, GetWindowSize()));
  zoom_layer_->SetMaskLayer(zoom_mask_->layer());

  host_widget_->AddObserver(this);
 }
