RenderWidgetHostViewAura::RenderWidgetHostViewAura(RenderWidgetHost* host,
                                                   bool is_guest_view_hack)
    : host_(RenderWidgetHostImpl::From(host)),
      window_(nullptr),
      in_shutdown_(false),
      in_bounds_changed_(false),
      popup_parent_host_view_(nullptr),
      popup_child_host_view_(nullptr),
      is_loading_(false),
      has_composition_text_(false),
      background_color_(SK_ColorWHITE),
      needs_begin_frames_(false),
      needs_flush_input_(false),
      added_frame_observer_(false),
      cursor_visibility_state_in_renderer_(UNKNOWN),
#if defined(OS_WIN)
      legacy_render_widget_host_HWND_(nullptr),
      legacy_window_destroyed_(false),
      virtual_keyboard_requested_(false),
#endif
      has_snapped_to_boundary_(false),
       is_guest_view_hack_(is_guest_view_hack),
       device_scale_factor_(0.0f),
       event_handler_(new RenderWidgetHostViewEventHandler(host_, this, this)),
       weak_ptr_factory_(this) {
   if (!is_guest_view_hack_)
     host_->SetView(this);

  if (GetTextInputManager())
    GetTextInputManager()->AddObserver(this);

  bool overscroll_enabled = base::CommandLine::ForCurrentProcess()->
      GetSwitchValueASCII(switches::kOverscrollHistoryNavigation) != "0";
  SetOverscrollControllerEnabled(overscroll_enabled);

  selection_controller_client_.reset(
      new TouchSelectionControllerClientAura(this));
  CreateSelectionController();

  RenderViewHost* rvh = RenderViewHost::From(host_);
  if (rvh) {
    ignore_result(rvh->GetWebkitPreferences());
  }
}
