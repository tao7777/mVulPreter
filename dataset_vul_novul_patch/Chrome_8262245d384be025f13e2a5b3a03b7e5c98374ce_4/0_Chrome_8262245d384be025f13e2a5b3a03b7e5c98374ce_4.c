RenderThread::~RenderThread() {
  FOR_EACH_OBSERVER(
      RenderProcessObserver, observers_, OnRenderProcessShutdown());

  if (web_database_observer_impl_.get())
     web_database_observer_impl_->WaitForAllDatabasesToClose();
 
  RemoveFilter(devtools_agent_message_filter_.get());
  devtools_agent_message_filter_ = NULL;

   RemoveFilter(audio_input_message_filter_.get());
   audio_input_message_filter_ = NULL;
 
  RemoveFilter(audio_message_filter_.get());
  audio_message_filter_ = NULL;

  RemoveFilter(vc_manager_->video_capture_message_filter());

  RemoveFilter(db_message_filter_.get());
  db_message_filter_ = NULL;

  if (file_thread_.get())
    file_thread_->Stop();

  if (webkit_client_.get())
    WebKit::shutdown();

  lazy_tls.Pointer()->Set(NULL);

#if defined(OS_WIN)
  PluginChannelBase::CleanupChannels();
  if (RenderProcessImpl::InProcessPlugins())
    CoUninitialize();
#endif
}
