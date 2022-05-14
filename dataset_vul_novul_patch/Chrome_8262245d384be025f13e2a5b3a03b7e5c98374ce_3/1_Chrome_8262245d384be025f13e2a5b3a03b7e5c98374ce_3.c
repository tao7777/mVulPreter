void RenderThread::Init() {
  TRACE_EVENT_BEGIN_ETW("RenderThread::Init", 0, "");

#if defined(OS_MACOSX)
  WebKit::WebView::setUseExternalPopupMenus(true);
#endif

  lazy_tls.Pointer()->Set(this);
#if defined(OS_WIN)
  if (RenderProcessImpl::InProcessPlugins())
    CoInitialize(0);
#endif

  suspend_webkit_shared_timer_ = true;
  notify_webkit_of_modal_loop_ = true;
  plugin_refresh_allowed_ = true;
  widget_count_ = 0;
  hidden_widget_count_ = 0;
  idle_notification_delay_in_s_ = kInitialIdleHandlerDelayS;
  task_factory_.reset(new ScopedRunnableMethodFactory<RenderThread>(this));

  appcache_dispatcher_.reset(new AppCacheDispatcher(this));
  indexed_db_dispatcher_.reset(new IndexedDBDispatcher());

  db_message_filter_ = new DBMessageFilter();
  AddFilter(db_message_filter_.get());

  vc_manager_ = new VideoCaptureImplManager();
  AddFilter(vc_manager_->video_capture_message_filter());

  audio_input_message_filter_ = new AudioInputMessageFilter();
  AddFilter(audio_input_message_filter_.get());

   audio_message_filter_ = new AudioMessageFilter();
   AddFilter(audio_message_filter_.get());
 
   content::GetContentClient()->renderer()->RenderThreadStarted();
 
   TRACE_EVENT_END_ETW("RenderThread::Init", 0, "");
}
