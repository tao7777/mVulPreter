RenderView::RenderView(RenderThreadBase* render_thread,
                       gfx::NativeViewId parent_hwnd,
                       int32 opener_id,
                       const RendererPreferences& renderer_prefs,
                       const WebPreferences& webkit_prefs,
                       SharedRenderViewCounter* counter,
                       int32 routing_id,
                       int64 session_storage_namespace_id,
                       const string16& frame_name)
    : RenderWidget(render_thread, WebKit::WebPopupTypeNone),
      webkit_preferences_(webkit_prefs),
      send_content_state_immediately_(false),
      enabled_bindings_(0),
      send_preferred_size_changes_(false),
      is_loading_(false),
      navigation_gesture_(NavigationGestureUnknown),
      opened_by_user_gesture_(true),
      opener_suppressed_(false),
      page_id_(-1),
      last_page_id_sent_to_browser_(-1),
      history_list_offset_(-1),
      history_list_length_(0),
      target_url_status_(TARGET_NONE),
      ALLOW_THIS_IN_INITIALIZER_LIST(pepper_delegate_(this)),
      ALLOW_THIS_IN_INITIALIZER_LIST(accessibility_method_factory_(this)),
      ALLOW_THIS_IN_INITIALIZER_LIST(cookie_jar_(this)),
      geolocation_dispatcher_(NULL),
      speech_input_dispatcher_(NULL),
      device_orientation_dispatcher_(NULL),
      accessibility_ack_pending_(false),
      p2p_socket_dispatcher_(NULL),
      session_storage_namespace_id_(session_storage_namespace_id) {
  routing_id_ = routing_id;
  if (opener_id != MSG_ROUTING_NONE)
    opener_id_ = opener_id;

  webwidget_ = WebView::create(this);

  if (counter) {
    shared_popup_counter_ = counter;
    shared_popup_counter_->data++;
    decrement_shared_popup_at_destruction_ = true;
  } else {
    shared_popup_counter_ = new SharedRenderViewCounter(0);
    decrement_shared_popup_at_destruction_ = false;
  }

  notification_provider_ = new NotificationProvider(this);

  render_thread_->AddRoute(routing_id_, this);
  AddRef();

  if (opener_id == MSG_ROUTING_NONE) {
    did_show_ = true;
    CompleteInit(parent_hwnd);
  }

  g_view_map.Get().insert(std::make_pair(webview(), this));
  webkit_preferences_.Apply(webview());
  webview()->initializeMainFrame(this);
  if (!frame_name.empty())
    webview()->mainFrame()->setName(frame_name);
  webview()->settings()->setMinimumTimerInterval(
      is_hidden() ? webkit_glue::kBackgroundTabTimerInterval :
          webkit_glue::kForegroundTabTimerInterval);

  OnSetRendererPrefs(renderer_prefs);

  host_window_ = parent_hwnd;

  const CommandLine& command_line = *CommandLine::ForCurrentProcess();
  if (command_line.HasSwitch(switches::kEnableAccessibility))
    WebAccessibilityCache::enableAccessibility();

#if defined(ENABLE_P2P_APIS)
  p2p_socket_dispatcher_ = new P2PSocketDispatcher(this);
#endif
 
   new MHTMLGenerator(this);
 
   if (command_line.HasSwitch(switches::kEnableMediaStream)) {
     media_stream_impl_ = new MediaStreamImpl(
         RenderThread::current()->video_capture_impl_manager());
  }

  content::GetContentClient()->renderer()->RenderViewCreated(this);
}
