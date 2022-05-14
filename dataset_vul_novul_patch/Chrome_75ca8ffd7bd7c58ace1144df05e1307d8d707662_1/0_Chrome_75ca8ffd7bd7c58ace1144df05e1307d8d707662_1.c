WebContentsImpl::WebContentsImpl(BrowserContext* browser_context)
    : delegate_(NULL),
      controller_(this, browser_context),
      render_view_host_delegate_view_(NULL),
      created_with_opener_(false),
#if defined(OS_WIN)
      accessible_parent_(NULL),
#endif
      frame_tree_(new NavigatorImpl(&controller_, this),
                  this,
                  this,
                  this,
                  this),
      is_loading_(false),
      is_load_to_different_document_(false),
      crashed_status_(base::TERMINATION_STATUS_STILL_RUNNING),
      crashed_error_code_(0),
      waiting_for_response_(false),
      load_state_(net::LOAD_STATE_IDLE, base::string16()),
      upload_size_(0),
      upload_position_(0),
      is_resume_pending_(false),
      displayed_insecure_content_(false),
      has_accessed_initial_document_(false),
      theme_color_(SK_ColorTRANSPARENT),
      last_sent_theme_color_(SK_ColorTRANSPARENT),
      did_first_visually_non_empty_paint_(false),
      capturer_count_(0),
      should_normally_be_visible_(true),
      is_being_destroyed_(false),
      notify_disconnection_(false),
      dialog_manager_(NULL),
      is_showing_before_unload_dialog_(false),
      last_active_time_(base::TimeTicks::Now()),
      closed_by_user_gesture_(false),
      minimum_zoom_percent_(static_cast<int>(kMinimumZoomFactor * 100)),
      maximum_zoom_percent_(static_cast<int>(kMaximumZoomFactor * 100)),
      zoom_scroll_remainder_(0),
      render_view_message_source_(NULL),
      render_frame_message_source_(NULL),
      fullscreen_widget_routing_id_(MSG_ROUTING_NONE),
      fullscreen_widget_had_focus_at_shutdown_(false),
      is_subframe_(false),
      force_disable_overscroll_content_(false),
      last_dialog_suppressed_(false),
      geolocation_service_context_(new GeolocationServiceContext()),
      accessibility_mode_(
          BrowserAccessibilityStateImpl::GetInstance()->accessibility_mode()),
       audio_stream_monitor_(this),
       virtual_keyboard_requested_(false),
       page_scale_factor_is_one_(true),
      loading_weak_factory_(this),
      weak_factory_(this) {
   frame_tree_.SetFrameRemoveListener(
       base::Bind(&WebContentsImpl::OnFrameRemoved,
                  base::Unretained(this)));
#if defined(OS_ANDROID)
  media_web_contents_observer_.reset(new MediaWebContentsObserverAndroid(this));
#else
  media_web_contents_observer_.reset(new MediaWebContentsObserver(this));
#endif
  loader_io_thread_notifier_.reset(new LoaderIOThreadNotifier(this));
  wake_lock_service_context_.reset(new WakeLockServiceContext(this));
}
