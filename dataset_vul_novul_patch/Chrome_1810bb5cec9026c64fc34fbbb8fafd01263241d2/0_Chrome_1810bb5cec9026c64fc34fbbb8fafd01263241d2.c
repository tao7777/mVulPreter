void RenderThreadImpl::EnsureWebKitInitialized() {
  if (webkit_platform_support_)
    return;
 
   webkit_platform_support_.reset(new RendererWebKitPlatformSupportImpl);
   blink::initialize(webkit_platform_support_.get());
  main_thread_compositor_task_runner_ = base::MessageLoopProxy::current();
 
   v8::Isolate* isolate = blink::mainThreadIsolate();
 
  isolate->SetCounterFunction(base::StatsTable::FindLocation);
  isolate->SetCreateHistogramFunction(CreateHistogram);
  isolate->SetAddHistogramSampleFunction(AddHistogramSample);

  const CommandLine& command_line = *CommandLine::ForCurrentProcess();

  bool enable = !command_line.HasSwitch(switches::kDisableThreadedCompositing);
  if (enable) {
#if defined(OS_ANDROID)
    if (SynchronousCompositorFactory* factory =
        SynchronousCompositorFactory::GetInstance())
      compositor_message_loop_proxy_ =
          factory->GetCompositorMessageLoop();
#endif
    if (!compositor_message_loop_proxy_.get()) {
      compositor_thread_.reset(new base::Thread("Compositor"));
      compositor_thread_->Start();
#if defined(OS_ANDROID)
      compositor_thread_->SetPriority(base::kThreadPriority_Display);
#endif
      compositor_message_loop_proxy_ =
          compositor_thread_->message_loop_proxy();
      compositor_message_loop_proxy_->PostTask(
          FROM_HERE,
          base::Bind(base::IgnoreResult(&ThreadRestrictions::SetIOAllowed),
                     false));
    }

    InputHandlerManagerClient* input_handler_manager_client = NULL;
#if defined(OS_ANDROID)
    if (SynchronousCompositorFactory* factory =
        SynchronousCompositorFactory::GetInstance()) {
      input_handler_manager_client = factory->GetInputHandlerManagerClient();
    }
#endif
    if (!input_handler_manager_client) {
      input_event_filter_ =
          new InputEventFilter(this,
                               main_thread_compositor_task_runner_,
                               compositor_message_loop_proxy_);
      AddFilter(input_event_filter_.get());
      input_handler_manager_client = input_event_filter_.get();
    }
    input_handler_manager_.reset(
        new InputHandlerManager(compositor_message_loop_proxy_,
                                input_handler_manager_client));
  }

  scoped_refptr<base::MessageLoopProxy> output_surface_loop;
  if (enable)
    output_surface_loop = compositor_message_loop_proxy_;
  else
    output_surface_loop = base::MessageLoopProxy::current();

  compositor_output_surface_filter_ =
      CompositorOutputSurface::CreateFilter(output_surface_loop.get());
  AddFilter(compositor_output_surface_filter_.get());

  RenderThreadImpl::RegisterSchemes();

  EnableBlinkPlatformLogChannels(
      command_line.GetSwitchValueASCII(switches::kBlinkPlatformLogChannels));

  SetRuntimeFeaturesDefaultsAndUpdateFromArgs(command_line);

  if (!media::IsMediaLibraryInitialized()) {
    WebRuntimeFeatures::enableWebAudio(false);
  }

  FOR_EACH_OBSERVER(RenderProcessObserver, observers_, WebKitInitialized());

  devtools_agent_message_filter_ = new DevToolsAgentFilter();
  AddFilter(devtools_agent_message_filter_.get());

  if (GetContentClient()->renderer()->RunIdleHandlerWhenWidgetsHidden())
    ScheduleIdleHandler(kLongIdleHandlerDelayMs);

  cc_blink::SetSharedMemoryAllocationFunction(AllocateSharedMemoryFunction);

  if (!command_line.HasSwitch(switches::kEnableDeferredImageDecoding) &&
      !is_impl_side_painting_enabled_)
    SkGraphics::SetImageCacheByteLimit(0u);

  SkGraphics::SetImageCacheSingleAllocationByteLimit(
      kImageCacheSingleAllocationByteLimit);

  if (command_line.HasSwitch(switches::kMemoryMetrics)) {
    memory_observer_.reset(new MemoryObserver());
    message_loop()->AddTaskObserver(memory_observer_.get());
  }
}
