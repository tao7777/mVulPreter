void RenderViewTest::SetUp() {
  blink::initialize(blink_platform_impl_.Get());

  content_client_.reset(CreateContentClient());
  content_browser_client_.reset(CreateContentBrowserClient());
  content_renderer_client_.reset(CreateContentRendererClient());
  SetContentClient(content_client_.get());
  SetBrowserClientForTesting(content_browser_client_.get());
  SetRendererClientForTesting(content_renderer_client_.get());

  if (!render_thread_)
    render_thread_.reset(new MockRenderThread());
  render_thread_->set_routing_id(kRouteId);
  render_thread_->set_surface_id(kSurfaceId);
  render_thread_->set_new_window_routing_id(kNewWindowRouteId);
  render_thread_->set_new_frame_routing_id(kNewFrameRouteId);

#if defined(OS_MACOSX)
  autorelease_pool_.reset(new base::mac::ScopedNSAutoreleasePool());
#endif
  command_line_.reset(new base::CommandLine(base::CommandLine::NO_PROGRAM));
  params_.reset(new MainFunctionParams(*command_line_));
  platform_.reset(new RendererMainPlatformDelegate(*params_));
  platform_->PlatformInitialize();

  std::string flags("--expose-gc");
  v8::V8::SetFlagsFromString(flags.c_str(), static_cast<int>(flags.size()));

  RenderThreadImpl::RegisterSchemes();

  if (!ui::ResourceBundle::HasSharedInstance())
    ui::ResourceBundle::InitSharedInstanceWithLocale(
        "en-US", NULL, ui::ResourceBundle::DO_NOT_LOAD_COMMON_RESOURCES);

  compositor_deps_.reset(new FakeCompositorDependencies);
  mock_process_.reset(new MockRenderProcess);

  ViewMsg_New_Params view_params;
  view_params.opener_frame_route_id = MSG_ROUTING_NONE;
  view_params.window_was_created_with_opener = false;
  view_params.renderer_preferences = RendererPreferences();
  view_params.web_preferences = WebPreferences();
  view_params.view_id = kRouteId;
  view_params.main_frame_routing_id = kMainFrameRouteId;
  view_params.surface_id = kSurfaceId;
  view_params.session_storage_namespace_id = kInvalidSessionStorageNamespaceId;
  view_params.swapped_out = false;
  view_params.replicated_frame_state = FrameReplicationState();
  view_params.proxy_routing_id = MSG_ROUTING_NONE;
  view_params.hidden = false;
  view_params.never_visible = false;
  view_params.next_page_id = 1;
  view_params.initial_size = *InitialSizeParams();
  view_params.enable_auto_resize = false;
   view_params.min_size = gfx::Size();
   view_params.max_size = gfx::Size();
 
#if !defined(OS_IOS)
  InitializeMojo();
#endif

   RenderViewImpl* view =
       RenderViewImpl::Create(compositor_deps_.get(), view_params, false);
  view_ = view;
}
