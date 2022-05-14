void RenderThreadImpl::Init(
    const scoped_refptr<base::SingleThreadTaskRunner>& resource_task_queue) {
  TRACE_EVENT0("startup", "RenderThreadImpl::Init");

  base::trace_event::TraceLog::GetInstance()->SetThreadSortIndex(
      base::PlatformThread::CurrentId(),
      kTraceEventRendererMainThreadSortIndex);

#if BUILDFLAG(USE_EXTERNAL_POPUP_MENU)
  blink::WebView::SetUseExternalPopupMenus(true);
#endif

  lazy_tls.Pointer()->Set(this);

  ChildProcess::current()->set_main_thread(this);

  metrics::InitializeSingleSampleMetricsFactory(
      base::BindRepeating(&CreateSingleSampleMetricsProvider,
                          message_loop()->task_runner(), GetConnector()));

  gpu_ = ui::Gpu::Create(
      GetConnector(),
      IsRunningInMash() ? ui::mojom::kServiceName : mojom::kBrowserServiceName,
      GetIOTaskRunner());

  viz::mojom::SharedBitmapAllocationNotifierPtr
      shared_bitmap_allocation_notifier_ptr;
  GetConnector()->BindInterface(
      mojom::kBrowserServiceName,
      mojo::MakeRequest(&shared_bitmap_allocation_notifier_ptr));
  shared_bitmap_manager_ = std::make_unique<viz::ClientSharedBitmapManager>(
      viz::mojom::ThreadSafeSharedBitmapAllocationNotifierPtr::Create(
          shared_bitmap_allocation_notifier_ptr.PassInterface(),
          GetChannel()->ipc_task_runner_refptr()));

  notification_dispatcher_ =
      new NotificationDispatcher(thread_safe_sender());
  AddFilter(notification_dispatcher_->GetFilter());

  resource_dispatcher_.reset(new ResourceDispatcher(
      this, message_loop()->task_runner()));
  resource_message_filter_ =
      new ChildResourceMessageFilter(resource_dispatcher_.get());
  AddFilter(resource_message_filter_.get());
  quota_message_filter_ =
      new QuotaMessageFilter(thread_safe_sender());
  quota_dispatcher_.reset(new QuotaDispatcher(thread_safe_sender(),
                                              quota_message_filter_.get()));

  AddFilter(quota_message_filter_->GetFilter());

  auto registry = std::make_unique<service_manager::BinderRegistry>();
  BlinkInterfaceRegistryImpl interface_registry(registry->GetWeakPtr());

  InitializeWebKit(resource_task_queue, &interface_registry);
  blink_initialized_time_ = base::TimeTicks::Now();

  webkit_shared_timer_suspended_ = false;
  widget_count_ = 0;
  hidden_widget_count_ = 0;
  idle_notification_delay_in_ms_ = kInitialIdleHandlerDelayMs;
  idle_notifications_to_skip_ = 0;

  appcache_dispatcher_.reset(
      new AppCacheDispatcher(Get(), new AppCacheFrontendImpl()));
  dom_storage_dispatcher_.reset(new DomStorageDispatcher());
  main_thread_indexed_db_dispatcher_.reset(new IndexedDBDispatcher());
  main_thread_cache_storage_dispatcher_.reset(
      new CacheStorageDispatcher(thread_safe_sender()));
  file_system_dispatcher_.reset(new FileSystemDispatcher());

  resource_dispatch_throttler_.reset(new ResourceDispatchThrottler(
      static_cast<RenderThread*>(this), renderer_scheduler_.get(),
      base::TimeDelta::FromSecondsD(kThrottledResourceRequestFlushPeriodS),
      kMaxResourceRequestsPerFlushWhenThrottled));
  resource_dispatcher_->set_message_sender(resource_dispatch_throttler_.get());

  blob_message_filter_ = new BlobMessageFilter(GetFileThreadTaskRunner());
  AddFilter(blob_message_filter_.get());
  vc_manager_.reset(new VideoCaptureImplManager());

  browser_plugin_manager_.reset(new BrowserPluginManager());
  AddObserver(browser_plugin_manager_.get());

#if BUILDFLAG(ENABLE_WEBRTC)
  peer_connection_tracker_.reset(new PeerConnectionTracker());
  AddObserver(peer_connection_tracker_.get());

  p2p_socket_dispatcher_ = new P2PSocketDispatcher(GetIOTaskRunner().get());
  AddFilter(p2p_socket_dispatcher_.get());

  peer_connection_factory_.reset(
      new PeerConnectionDependencyFactory(p2p_socket_dispatcher_.get()));

  aec_dump_message_filter_ = new AecDumpMessageFilter(
      GetIOTaskRunner(), message_loop()->task_runner());

  AddFilter(aec_dump_message_filter_.get());

#endif  // BUILDFLAG(ENABLE_WEBRTC)

  audio_input_message_filter_ = new AudioInputMessageFilter(GetIOTaskRunner());
  AddFilter(audio_input_message_filter_.get());

  scoped_refptr<AudioMessageFilter> audio_message_filter;
  if (!base::FeatureList::IsEnabled(
          features::kUseMojoAudioOutputStreamFactory)) {
    audio_message_filter =
        base::MakeRefCounted<AudioMessageFilter>(GetIOTaskRunner());
    AddFilter(audio_message_filter.get());
  }

  audio_ipc_factory_.emplace(std::move(audio_message_filter),
                             GetIOTaskRunner());

  midi_message_filter_ = new MidiMessageFilter(GetIOTaskRunner());
  AddFilter(midi_message_filter_.get());

  AddFilter((new CacheStorageMessageFilter(thread_safe_sender()))->GetFilter());

  AddFilter((new ServiceWorkerContextMessageFilter())->GetFilter());


#if defined(USE_AURA)
  if (IsRunningInMash()) {
    CreateRenderWidgetWindowTreeClientFactory(GetServiceManagerConnection());
  }
#endif

  registry->AddInterface(base::Bind(&SharedWorkerFactoryImpl::Create),
                         base::ThreadTaskRunnerHandle::Get());
  GetServiceManagerConnection()->AddConnectionFilter(
      std::make_unique<SimpleConnectionFilter>(std::move(registry)));

  {
    auto registry_with_source_info =
        std::make_unique<service_manager::BinderRegistryWithArgs<
            const service_manager::BindSourceInfo&>>();
    registry_with_source_info->AddInterface(
        base::Bind(&CreateFrameFactory), base::ThreadTaskRunnerHandle::Get());
    GetServiceManagerConnection()->AddConnectionFilter(
        std::make_unique<SimpleConnectionFilterWithSourceInfo>(
            std::move(registry_with_source_info)));
  }

  GetContentClient()->renderer()->RenderThreadStarted();

  StartServiceManagerConnection();

  GetAssociatedInterfaceRegistry()->AddInterface(
      base::Bind(&RenderThreadImpl::OnRendererInterfaceRequest,
                 base::Unretained(this)));

  InitSkiaEventTracer();
  base::trace_event::MemoryDumpManager::GetInstance()->RegisterDumpProvider(
      skia::SkiaMemoryDumpProvider::GetInstance(), "Skia", nullptr);

  const base::CommandLine& command_line =
      *base::CommandLine::ForCurrentProcess();

#if defined(ENABLE_IPC_FUZZER)
  if (command_line.HasSwitch(switches::kIpcDumpDirectory)) {
    base::FilePath dump_directory =
        command_line.GetSwitchValuePath(switches::kIpcDumpDirectory);
    IPC::ChannelProxy::OutgoingMessageFilter* filter =
        LoadExternalIPCDumper(dump_directory);
    GetChannel()->set_outgoing_message_filter(filter);
  }
#endif

  cc::SetClientNameForMetrics("Renderer");

  is_threaded_animation_enabled_ =
      !command_line.HasSwitch(cc::switches::kDisableThreadedAnimation);

  is_zero_copy_enabled_ = command_line.HasSwitch(switches::kEnableZeroCopy);
  is_partial_raster_enabled_ =
      !command_line.HasSwitch(switches::kDisablePartialRaster);
  is_gpu_memory_buffer_compositor_resources_enabled_ = command_line.HasSwitch(
      switches::kEnableGpuMemoryBufferCompositorResources);

#if defined(OS_MACOSX)
  is_elastic_overscroll_enabled_ = true;
#else
  is_elastic_overscroll_enabled_ = false;
#endif

  std::string image_texture_target_string =
      command_line.GetSwitchValueASCII(switches::kContentImageTextureTarget);
  buffer_to_texture_target_map_ =
      viz::StringToBufferToTextureTargetMap(image_texture_target_string);

  if (command_line.HasSwitch(switches::kDisableLCDText)) {
    is_lcd_text_enabled_ = false;
  } else if (command_line.HasSwitch(switches::kEnableLCDText)) {
    is_lcd_text_enabled_ = true;
  } else {
#if defined(OS_ANDROID)
    is_lcd_text_enabled_ = false;
#else
    is_lcd_text_enabled_ = true;
#endif
  }

  if (command_line.HasSwitch(switches::kDisableGpuCompositing))
    is_gpu_compositing_disabled_ = true;

  is_gpu_rasterization_forced_ =
      command_line.HasSwitch(switches::kForceGpuRasterization);
  is_async_worker_context_enabled_ =
      command_line.HasSwitch(switches::kEnableGpuAsyncWorkerContext);

  if (command_line.HasSwitch(switches::kGpuRasterizationMSAASampleCount)) {
    std::string string_value = command_line.GetSwitchValueASCII(
        switches::kGpuRasterizationMSAASampleCount);
    bool parsed_msaa_sample_count =
        base::StringToInt(string_value, &gpu_rasterization_msaa_sample_count_);
    DCHECK(parsed_msaa_sample_count) << string_value;
    DCHECK_GE(gpu_rasterization_msaa_sample_count_, 0);
  } else {
    gpu_rasterization_msaa_sample_count_ = -1;
  }

  if (command_line.HasSwitch(switches::kDisableDistanceFieldText)) {
    is_distance_field_text_enabled_ = false;
  } else if (command_line.HasSwitch(switches::kEnableDistanceFieldText)) {
    is_distance_field_text_enabled_ = true;
  } else {
    is_distance_field_text_enabled_ = false;
  }

  WebRuntimeFeatures::EnableCompositorImageAnimations(
      command_line.HasSwitch(switches::kEnableCompositorImageAnimations));

  media::InitializeMediaLibrary();

#if defined(OS_ANDROID)
  if (!command_line.HasSwitch(switches::kDisableAcceleratedVideoDecode) &&
      media::MediaCodecUtil::IsMediaCodecAvailable()) {
    media::EnablePlatformDecoderSupport();
  }
#endif

  memory_pressure_listener_.reset(new base::MemoryPressureListener(
      base::Bind(&RenderThreadImpl::OnMemoryPressure, base::Unretained(this)),
      base::Bind(&RenderThreadImpl::OnSyncMemoryPressure,
                 base::Unretained(this))));

  if (base::FeatureList::IsEnabled(features::kMemoryCoordinator)) {
    base::MemoryPressureListener::SetNotificationsSuppressed(true);

    mojom::MemoryCoordinatorHandlePtr parent_coordinator;
    GetConnector()->BindInterface(mojom::kBrowserServiceName,
                                  mojo::MakeRequest(&parent_coordinator));
    memory_coordinator_ = CreateChildMemoryCoordinator(
        std::move(parent_coordinator), this);
  }

  int num_raster_threads = 0;
  std::string string_value =
      command_line.GetSwitchValueASCII(switches::kNumRasterThreads);
  bool parsed_num_raster_threads =
      base::StringToInt(string_value, &num_raster_threads);
  DCHECK(parsed_num_raster_threads) << string_value;
  DCHECK_GT(num_raster_threads, 0);

  categorized_worker_pool_->Start(num_raster_threads);

  discardable_memory::mojom::DiscardableSharedMemoryManagerPtr manager_ptr;
  if (IsRunningInMash()) {
#if defined(USE_AURA)
    GetServiceManagerConnection()->GetConnector()->BindInterface(
        ui::mojom::kServiceName, &manager_ptr);
#else
    NOTREACHED();
#endif
  } else {
    ChildThread::Get()->GetConnector()->BindInterface(
        mojom::kBrowserServiceName, mojo::MakeRequest(&manager_ptr));
  }

  discardable_shared_memory_manager_ = std::make_unique<
      discardable_memory::ClientDiscardableSharedMemoryManager>(
      std::move(manager_ptr), GetIOTaskRunner());

  base::DiscardableMemoryAllocator::SetInstance(
      discardable_shared_memory_manager_.get());

  GetConnector()->BindInterface(mojom::kBrowserServiceName,
                                mojo::MakeRequest(&storage_partition_service_));

#if defined(OS_LINUX)
  ChildProcess::current()->SetIOThreadPriority(base::ThreadPriority::DISPLAY);
  ChildThreadImpl::current()->SetThreadPriority(
      categorized_worker_pool_->background_worker_thread_id(),
      base::ThreadPriority::BACKGROUND);
#endif

  process_foregrounded_count_ = 0;
  needs_to_record_first_active_paint_ = false;
  was_backgrounded_time_ = base::TimeTicks::Min();

  base::MemoryCoordinatorClientRegistry::GetInstance()->Register(this);

   if (!command_line.HasSwitch(switches::kSingleProcess))
     base::SequencedWorkerPool::EnableForProcess();
 
   GetConnector()->BindInterface(mojom::kBrowserServiceName,
                                 mojo::MakeRequest(&frame_sink_provider_));
 
  if (!is_gpu_compositing_disabled_) {
    GetConnector()->BindInterface(
        mojom::kBrowserServiceName,
        mojo::MakeRequest(&compositing_mode_reporter_));

    viz::mojom::CompositingModeWatcherPtr watcher_ptr;
    compositing_mode_watcher_binding_.Bind(mojo::MakeRequest(&watcher_ptr));
    compositing_mode_reporter_->AddCompositingModeWatcher(
        std::move(watcher_ptr));
  }
}
