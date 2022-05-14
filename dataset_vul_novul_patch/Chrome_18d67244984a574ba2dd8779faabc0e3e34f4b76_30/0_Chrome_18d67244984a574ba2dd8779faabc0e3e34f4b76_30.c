RenderWidgetHostViewAura::RenderWidgetHostViewAura(RenderWidgetHost* host)
    : host_(RenderWidgetHostImpl::From(host)),
      ALLOW_THIS_IN_INITIALIZER_LIST(window_(new aura::Window(this))),
      in_shutdown_(false),
      is_fullscreen_(false),
      popup_parent_host_view_(NULL),
      popup_child_host_view_(NULL),
      is_loading_(false),
      text_input_type_(ui::TEXT_INPUT_TYPE_NONE),
      can_compose_inline_(true),
       has_composition_text_(false),
       device_scale_factor_(1.0f),
       current_surface_(0),
       paint_canvas_(NULL),
       synthetic_move_sent_(false),
       accelerated_compositing_state_changed_(false),
      can_lock_compositor_(YES) {
  host_->SetView(this);
  window_observer_.reset(new WindowObserver(this));
  window_->AddObserver(window_observer_.get());
  aura::client::SetTooltipText(window_, &tooltip_);
  aura::client::SetActivationDelegate(window_, this);
  gfx::Screen::GetScreenFor(window_)->AddObserver(this);
}
