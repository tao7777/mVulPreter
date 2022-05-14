void RenderThreadImpl::Shutdown() {
  FOR_EACH_OBSERVER(
      RenderProcessObserver, observers_, OnRenderProcessShutdown());

  ChildThread::Shutdown();

  if (memory_observer_) {
    message_loop()->RemoveTaskObserver(memory_observer_.get());
    memory_observer_.reset();
  }
 
   if (webkit_platform_support_) {
    // WaitForAllDatabasesToClose might run a nested message loop. To avoid
    // processing timer events while we're already in the process of shutting
    // down blink, put a ScopePageLoadDeferrer on the stack.
    WebView::willEnterModalLoop();
     webkit_platform_support_->web_database_observer_impl()->
         WaitForAllDatabasesToClose();
    WebView::didExitModalLoop();
   }
 
  if (devtools_agent_message_filter_.get()) {
    RemoveFilter(devtools_agent_message_filter_.get());
    devtools_agent_message_filter_ = NULL;
  }

  RemoveFilter(audio_input_message_filter_.get());
  audio_input_message_filter_ = NULL;

  RemoveFilter(audio_message_filter_.get());
  audio_message_filter_ = NULL;

#if defined(ENABLE_WEBRTC)
  RTCPeerConnectionHandler::DestructAllHandlers();

  peer_connection_factory_.reset();
#endif
  RemoveFilter(vc_manager_->video_capture_message_filter());
  vc_manager_.reset();

  RemoveFilter(db_message_filter_.get());
  db_message_filter_ = NULL;

  if (file_thread_)
    file_thread_->Stop();

  if (compositor_output_surface_filter_.get()) {
    RemoveFilter(compositor_output_surface_filter_.get());
    compositor_output_surface_filter_ = NULL;
  }

  media_thread_.reset();
  compositor_thread_.reset();
  input_handler_manager_.reset();
  if (input_event_filter_.get()) {
    RemoveFilter(input_event_filter_.get());
    input_event_filter_ = NULL;
  }

  embedded_worker_dispatcher_.reset();

  main_thread_indexed_db_dispatcher_.reset();

  if (webkit_platform_support_)
    blink::shutdown();

  lazy_tls.Pointer()->Set(NULL);

#if defined(OS_WIN)
  NPChannelBase::CleanupChannels();
#endif
}
