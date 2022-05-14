void RenderViewTest::SetUp() {
  if (!GetContentClient()->renderer())
    GetContentClient()->set_renderer(&mock_content_renderer_client_);

  if (!render_thread_.get())
    render_thread_.reset(new MockRenderThread());
  render_thread_->set_routing_id(kRouteId);
  render_thread_->set_surface_id(kSurfaceId);
  render_thread_->set_new_window_routing_id(kNewWindowRouteId);

  command_line_.reset(new CommandLine(CommandLine::NO_PROGRAM));
  params_.reset(new content::MainFunctionParams(*command_line_));
  platform_.reset(new RendererMainPlatformDelegate(*params_));
  platform_->PlatformInitialize();

   webkit_glue::SetJavaScriptFlags(" --expose-gc");
   WebKit::initialize(&webkit_platform_support_);
 
  // Ensure that we register any necessary schemes when initializing WebKit,
  // since we are using a MockRenderThread.
  RenderThreadImpl::RegisterSchemes();

   mock_process_.reset(new MockRenderProcess);
 
  RenderViewImpl* view = RenderViewImpl::Create(
      0,
      kOpenerId,
      content::RendererPreferences(),
      WebPreferences(),
      new SharedRenderViewCounter(0),
      kRouteId,
      kSurfaceId,
      kInvalidSessionStorageNamespaceId,
      string16(),
      1,
      WebKit::WebScreenInfo(),
      false);
  view->AddRef();
  view_ = view;

  mock_keyboard_.reset(new MockKeyboard());
}
