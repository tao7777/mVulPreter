RenderFrameImpl::RenderFrameImpl(const CreateParams& params)
    : frame_(NULL),
      is_subframe_(false),
      is_local_root_(false),
      render_view_(params.render_view->AsWeakPtr()),
      routing_id_(params.routing_id),
      is_swapped_out_(false),
      render_frame_proxy_(NULL),
      is_detaching_(false),
      proxy_routing_id_(MSG_ROUTING_NONE),
#if defined(ENABLE_PLUGINS)
      plugin_power_saver_helper_(NULL),
#endif
      cookie_jar_(this),
      selection_text_offset_(0),
      selection_range_(gfx::Range::InvalidRange()),
      handling_select_range_(false),
      notification_permission_dispatcher_(NULL),
      web_user_media_client_(NULL),
      media_permission_dispatcher_(NULL),
      midi_dispatcher_(NULL),
#if defined(OS_ANDROID)
      media_player_manager_(NULL),
#endif
#if defined(ENABLE_BROWSER_CDMS)
      cdm_manager_(NULL),
#endif
#if defined(VIDEO_HOLE)
      contains_media_player_(false),
#endif
      has_played_media_(false),
      devtools_agent_(nullptr),
      geolocation_dispatcher_(NULL),
      push_messaging_dispatcher_(NULL),
      presentation_dispatcher_(NULL),
      screen_orientation_dispatcher_(NULL),
      manifest_manager_(NULL),
      accessibility_mode_(AccessibilityModeOff),
      renderer_accessibility_(NULL),
      weak_factory_(this) {
  std::pair<RoutingIDFrameMap::iterator, bool> result =
      g_routing_id_frame_map.Get().insert(std::make_pair(routing_id_, this));
  CHECK(result.second) << "Inserting a duplicate item.";

  RenderThread::Get()->AddRoute(routing_id_, this);

  render_view_->RegisterRenderFrame(this);

#if defined(OS_ANDROID)
  new GinJavaBridgeDispatcher(this);
#endif

#if defined(ENABLE_PLUGINS)
  plugin_power_saver_helper_ = new PluginPowerSaverHelper(this);
 #endif
 
   manifest_manager_ = new ManifestManager(this);

  GetServiceRegistry()->ConnectToRemoteService(mojo::GetProxy(&mojo_shell_));
 }
