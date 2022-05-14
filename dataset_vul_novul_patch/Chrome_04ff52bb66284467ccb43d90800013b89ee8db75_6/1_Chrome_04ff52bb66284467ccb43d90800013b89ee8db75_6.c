void RenderProcessHostImpl::CreateMessageFilters() {
  DCHECK_CURRENTLY_ON(BrowserThread::UI);
  AddFilter(new ResourceSchedulerFilter(GetID()));
  MediaInternals* media_internals = MediaInternals::GetInstance();
  scoped_refptr<BrowserPluginMessageFilter> bp_message_filter(
      new BrowserPluginMessageFilter(GetID()));
  AddFilter(bp_message_filter.get());

  scoped_refptr<net::URLRequestContextGetter> request_context(
      storage_partition_impl_->GetURLRequestContext());
  scoped_refptr<RenderMessageFilter> render_message_filter(
      new RenderMessageFilter(
          GetID(), GetBrowserContext(), request_context.get(),
          widget_helper_.get(), media_internals,
          storage_partition_impl_->GetDOMStorageContext(),
          storage_partition_impl_->GetCacheStorageContext()));
  AddFilter(render_message_filter.get());

  render_frame_message_filter_ = new RenderFrameMessageFilter(
      GetID(),
#if BUILDFLAG(ENABLE_PLUGINS)
      PluginServiceImpl::GetInstance(),
#else
      nullptr,
#endif
      GetBrowserContext(),
      request_context.get(),
      widget_helper_.get());
  AddFilter(render_frame_message_filter_.get());

  BrowserContext* browser_context = GetBrowserContext();
  ResourceContext* resource_context = browser_context->GetResourceContext();

  scoped_refptr<net::URLRequestContextGetter> media_request_context(
      GetStoragePartition()->GetMediaURLRequestContext());

  ResourceMessageFilter::GetContextsCallback get_contexts_callback(
      base::Bind(&GetContexts, browser_context->GetResourceContext(),
                 request_context, media_request_context));

  scoped_refptr<ChromeBlobStorageContext> blob_storage_context =
      ChromeBlobStorageContext::GetFor(browser_context);

  resource_message_filter_ = new ResourceMessageFilter(
      GetID(), storage_partition_impl_->GetAppCacheService(),
      blob_storage_context.get(),
      storage_partition_impl_->GetFileSystemContext(),
      storage_partition_impl_->GetServiceWorkerContext(),
      get_contexts_callback);

  AddFilter(resource_message_filter_.get());

  media::AudioManager* audio_manager =
      BrowserMainLoop::GetInstance()->audio_manager();
  MediaStreamManager* media_stream_manager =
      BrowserMainLoop::GetInstance()->media_stream_manager();
  audio_input_renderer_host_ = new AudioInputRendererHost(
      GetID(), base::GetProcId(GetHandle()), audio_manager,
      media_stream_manager, AudioMirroringManager::GetInstance(),
       BrowserMainLoop::GetInstance()->user_input_monitor());
   AddFilter(audio_input_renderer_host_.get());
   audio_renderer_host_ = new AudioRendererHost(
      GetID(), audio_manager, AudioMirroringManager::GetInstance(),
      media_stream_manager,
       browser_context->GetResourceContext()->GetMediaDeviceIDSalt());
   AddFilter(audio_renderer_host_.get());
   AddFilter(
      new MidiHost(GetID(), BrowserMainLoop::GetInstance()->midi_service()));
  AddFilter(new AppCacheDispatcherHost(
      storage_partition_impl_->GetAppCacheService(), GetID()));
  AddFilter(new ClipboardMessageFilter(blob_storage_context));
  AddFilter(new DOMStorageMessageFilter(
      storage_partition_impl_->GetDOMStorageContext()));

#if BUILDFLAG(ENABLE_WEBRTC)
  peer_connection_tracker_host_ = new PeerConnectionTrackerHost(
      GetID(), webrtc_eventlog_host_.GetWeakPtr());
  AddFilter(peer_connection_tracker_host_.get());
  AddFilter(new MediaStreamDispatcherHost(
      GetID(), browser_context->GetResourceContext()->GetMediaDeviceIDSalt(),
      media_stream_manager));
  AddFilter(new MediaStreamTrackMetricsHost());
#endif
#if BUILDFLAG(ENABLE_PLUGINS)
  AddFilter(new PepperRendererConnection(GetID()));
#endif
  AddFilter(new SpeechRecognitionDispatcherHost(
      GetID(), storage_partition_impl_->GetURLRequestContext()));
  AddFilter(new FileAPIMessageFilter(
      GetID(), storage_partition_impl_->GetURLRequestContext(),
      storage_partition_impl_->GetFileSystemContext(),
      blob_storage_context.get(), StreamContext::GetFor(browser_context)));
  AddFilter(new BlobDispatcherHost(
      GetID(), blob_storage_context,
      make_scoped_refptr(storage_partition_impl_->GetFileSystemContext())));
  AddFilter(new FileUtilitiesMessageFilter(GetID()));
  AddFilter(
      new DatabaseMessageFilter(storage_partition_impl_->GetDatabaseTracker()));
#if defined(OS_MACOSX)
  AddFilter(new TextInputClientMessageFilter());
#elif defined(OS_WIN)
  AddFilter(new DWriteFontProxyMessageFilter());

  channel_->AddFilter(new FontCacheDispatcher());
#endif

  message_port_message_filter_ = new MessagePortMessageFilter(
      base::Bind(&RenderWidgetHelper::GetNextRoutingID,
                 base::Unretained(widget_helper_.get())));
  AddFilter(message_port_message_filter_.get());

  scoped_refptr<CacheStorageDispatcherHost> cache_storage_filter =
      new CacheStorageDispatcherHost();
  cache_storage_filter->Init(storage_partition_impl_->GetCacheStorageContext());
  AddFilter(cache_storage_filter.get());

  scoped_refptr<ServiceWorkerDispatcherHost> service_worker_filter =
      new ServiceWorkerDispatcherHost(
          GetID(), message_port_message_filter_.get(), resource_context);
  service_worker_filter->Init(
      storage_partition_impl_->GetServiceWorkerContext());
  AddFilter(service_worker_filter.get());

  AddFilter(new SharedWorkerMessageFilter(
      GetID(), resource_context,
      WorkerStoragePartition(
          storage_partition_impl_->GetURLRequestContext(),
          storage_partition_impl_->GetMediaURLRequestContext(),
          storage_partition_impl_->GetAppCacheService(),
          storage_partition_impl_->GetQuotaManager(),
          storage_partition_impl_->GetFileSystemContext(),
          storage_partition_impl_->GetDatabaseTracker(),
          storage_partition_impl_->GetIndexedDBContext(),
          storage_partition_impl_->GetServiceWorkerContext()),
      message_port_message_filter_.get()));

#if BUILDFLAG(ENABLE_WEBRTC)
  p2p_socket_dispatcher_host_ = new P2PSocketDispatcherHost(
      resource_context, request_context.get());
  AddFilter(p2p_socket_dispatcher_host_.get());
#endif

  AddFilter(new TraceMessageFilter(GetID()));
  AddFilter(new ResolveProxyMsgHelper(request_context.get()));
  AddFilter(new QuotaDispatcherHost(
      GetID(), storage_partition_impl_->GetQuotaManager(),
      GetContentClient()->browser()->CreateQuotaPermissionContext()));

  scoped_refptr<ServiceWorkerContextWrapper> service_worker_context(
      static_cast<ServiceWorkerContextWrapper*>(
          storage_partition_impl_->GetServiceWorkerContext()));
  notification_message_filter_ = new NotificationMessageFilter(
      GetID(), storage_partition_impl_->GetPlatformNotificationContext(),
      resource_context, service_worker_context, browser_context);
  AddFilter(notification_message_filter_.get());

  AddFilter(new ProfilerMessageFilter(PROCESS_TYPE_RENDERER));
  AddFilter(new HistogramMessageFilter());
  AddFilter(new MemoryMessageFilter(this));
  AddFilter(new PushMessagingMessageFilter(
      GetID(), storage_partition_impl_->GetServiceWorkerContext()));
#if defined(OS_ANDROID)
  AddFilter(new ScreenOrientationListenerAndroid());
  synchronous_compositor_filter_ =
      new SynchronousCompositorBrowserFilter(GetID());
  AddFilter(synchronous_compositor_filter_.get());
#endif
}
