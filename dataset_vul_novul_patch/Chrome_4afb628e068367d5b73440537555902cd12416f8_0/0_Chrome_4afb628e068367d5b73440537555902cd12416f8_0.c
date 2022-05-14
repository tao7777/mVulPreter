void CompositorImpl::InitializeVizLayerTreeFrameSink(
    scoped_refptr<ws::ContextProviderCommandBuffer> context_provider) {
  DCHECK(enable_viz_);

  pending_frames_ = 0;
  gpu_capabilities_ = context_provider->ContextCapabilities();
  scoped_refptr<base::SingleThreadTaskRunner> task_runner =
      base::ThreadTaskRunnerHandle::Get();

  auto root_params = viz::mojom::RootCompositorFrameSinkParams::New();

  root_params->send_swap_size_notifications = true;

  viz::mojom::CompositorFrameSinkAssociatedPtrInfo sink_info;
  root_params->compositor_frame_sink = mojo::MakeRequest(&sink_info);
  viz::mojom::CompositorFrameSinkClientRequest client_request =
      mojo::MakeRequest(&root_params->compositor_frame_sink_client);
  root_params->display_private = mojo::MakeRequest(&display_private_);
  display_client_ = std::make_unique<AndroidHostDisplayClient>(
      base::BindRepeating(&CompositorImpl::DidSwapBuffers,
                          weak_factory_.GetWeakPtr()),
      base::BindRepeating(
          &CompositorImpl::OnFatalOrSurfaceContextCreationFailure,
          weak_factory_.GetWeakPtr()));
  root_params->display_client =
       display_client_->GetBoundPtr(task_runner).PassInterface();
 
   viz::RendererSettings renderer_settings;
  renderer_settings.partial_swap_enabled = true;
   renderer_settings.allow_antialiasing = false;
   renderer_settings.highp_threshold_min = 2048;
   renderer_settings.requires_alpha_channel = requires_alpha_channel_;
  renderer_settings.initial_screen_size =
      display::Screen::GetScreen()
          ->GetDisplayNearestWindow(root_window_)
          .GetSizeInPixel();
  renderer_settings.use_skia_renderer = features::IsUsingSkiaRenderer();
  renderer_settings.color_space = display_color_space_;
  root_params->frame_sink_id = frame_sink_id_;
  root_params->widget = surface_handle_;
  root_params->gpu_compositing = true;
  root_params->renderer_settings = renderer_settings;

  GetHostFrameSinkManager()->CreateRootCompositorFrameSink(
      std::move(root_params));

  cc::mojo_embedder::AsyncLayerTreeFrameSink::InitParams params;
  params.compositor_task_runner = task_runner;
  params.gpu_memory_buffer_manager = BrowserMainLoop::GetInstance()
                                         ->gpu_channel_establish_factory()
                                         ->GetGpuMemoryBufferManager();
  params.pipes.compositor_frame_sink_associated_info = std::move(sink_info);
  params.pipes.client_request = std::move(client_request);
  params.enable_surface_synchronization = true;
  params.hit_test_data_provider =
      std::make_unique<viz::HitTestDataProviderDrawQuad>(
          false /* should_ask_for_child_region */,
          true /* root_accepts_events */);
  params.client_name = kBrowser;
  auto layer_tree_frame_sink =
      std::make_unique<cc::mojo_embedder::AsyncLayerTreeFrameSink>(
          std::move(context_provider), nullptr, &params);
  host_->SetLayerTreeFrameSink(std::move(layer_tree_frame_sink));
  display_private_->SetDisplayVisible(true);
  display_private_->Resize(size_);
  display_private_->SetDisplayColorSpace(display_color_space_,
                                         display_color_space_);
  display_private_->SetVSyncPaused(vsync_paused_);
}
