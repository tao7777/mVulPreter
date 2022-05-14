void RenderFrameHostImpl::RegisterMojoInterfaces() {
#if !defined(OS_ANDROID)
  registry_->AddInterface(base::Bind(&InstalledAppProviderImplDefault::Create));
#endif  // !defined(OS_ANDROID)

  PermissionControllerImpl* permission_controller =
      PermissionControllerImpl::FromBrowserContext(
          GetProcess()->GetBrowserContext());
  if (delegate_) {
    auto* geolocation_context = delegate_->GetGeolocationContext();
    if (geolocation_context) {
      geolocation_service_.reset(new GeolocationServiceImpl(
          geolocation_context, permission_controller, this));
      registry_->AddInterface(
          base::Bind(&GeolocationServiceImpl::Bind,
                     base::Unretained(geolocation_service_.get())));
    }
  }

  registry_->AddInterface<device::mojom::WakeLock>(base::Bind(
      &RenderFrameHostImpl::BindWakeLockRequest, base::Unretained(this)));

#if defined(OS_ANDROID)
  if (base::FeatureList::IsEnabled(features::kWebNfc)) {
    registry_->AddInterface<device::mojom::NFC>(base::Bind(
        &RenderFrameHostImpl::BindNFCRequest, base::Unretained(this)));
  }
#endif

  if (!permission_service_context_)
    permission_service_context_.reset(new PermissionServiceContext(this));

  registry_->AddInterface(
      base::Bind(&PermissionServiceContext::CreateService,
                 base::Unretained(permission_service_context_.get())));

  registry_->AddInterface(
      base::Bind(&RenderFrameHostImpl::BindPresentationServiceRequest,
                 base::Unretained(this)));

  registry_->AddInterface(
      base::Bind(&MediaSessionServiceImpl::Create, base::Unretained(this)));

  registry_->AddInterface(base::Bind(
      base::IgnoreResult(&RenderFrameHostImpl::CreateWebBluetoothService),
      base::Unretained(this)));

  registry_->AddInterface(base::BindRepeating(
      &RenderFrameHostImpl::CreateWebUsbService, base::Unretained(this)));

  registry_->AddInterface<media::mojom::InterfaceFactory>(
      base::Bind(&RenderFrameHostImpl::BindMediaInterfaceFactoryRequest,
                 base::Unretained(this)));

  registry_->AddInterface(base::BindRepeating(
      &RenderFrameHostImpl::CreateWebSocket, base::Unretained(this)));

  registry_->AddInterface(base::BindRepeating(
      &RenderFrameHostImpl::CreateDedicatedWorkerHostFactory,
      base::Unretained(this)));

  registry_->AddInterface(base::Bind(&SharedWorkerConnectorImpl::Create,
                                     process_->GetID(), routing_id_));

  registry_->AddInterface(base::BindRepeating(&device::GamepadMonitor::Create));

  registry_->AddInterface<device::mojom::VRService>(base::Bind(
      &WebvrServiceProvider::BindWebvrService, base::Unretained(this)));

  registry_->AddInterface(
      base::BindRepeating(&RenderFrameHostImpl::CreateAudioInputStreamFactory,
                          base::Unretained(this)));

  registry_->AddInterface(
      base::BindRepeating(&RenderFrameHostImpl::CreateAudioOutputStreamFactory,
                          base::Unretained(this)));

  registry_->AddInterface(
      base::Bind(&CreateFrameResourceCoordinator, base::Unretained(this)));

  if (BrowserMainLoop::GetInstance()) {
    MediaStreamManager* media_stream_manager =
        BrowserMainLoop::GetInstance()->media_stream_manager();
    registry_->AddInterface(
        base::Bind(&MediaDevicesDispatcherHost::Create, GetProcess()->GetID(),
                   GetRoutingID(), base::Unretained(media_stream_manager)),
         base::CreateSingleThreadTaskRunnerWithTraits({BrowserThread::IO}));
 
     registry_->AddInterface(
        base::BindRepeating(&MediaStreamDispatcherHost::Create,
                            GetProcess()->GetID(), GetRoutingID(),
                            base::Unretained(media_stream_manager)),
         base::CreateSingleThreadTaskRunnerWithTraits({BrowserThread::IO}));
   }
 
#if BUILDFLAG(ENABLE_MEDIA_REMOTING)
  registry_->AddInterface(base::Bind(&RemoterFactoryImpl::Bind,
                                     GetProcess()->GetID(), GetRoutingID()));
#endif  // BUILDFLAG(ENABLE_MEDIA_REMOTING)

  registry_->AddInterface(base::BindRepeating(
      &KeyboardLockServiceImpl::CreateMojoService, base::Unretained(this)));

  registry_->AddInterface(base::Bind(&ImageCaptureImpl::Create));

#if !defined(OS_ANDROID)
  if (base::FeatureList::IsEnabled(features::kWebAuth)) {
    registry_->AddInterface(
        base::Bind(&RenderFrameHostImpl::BindAuthenticatorRequest,
                   base::Unretained(this)));
    if (base::CommandLine::ForCurrentProcess()->HasSwitch(
            switches::kEnableWebAuthTestingAPI)) {
      auto* environment_singleton =
          ScopedVirtualAuthenticatorEnvironment::GetInstance();
      registry_->AddInterface(base::BindRepeating(
          &ScopedVirtualAuthenticatorEnvironment::AddBinding,
          base::Unretained(environment_singleton)));
    }
  }
#endif  // !defined(OS_ANDROID)

  sensor_provider_proxy_.reset(
      new SensorProviderProxyImpl(permission_controller, this));
  registry_->AddInterface(
      base::Bind(&SensorProviderProxyImpl::Bind,
                 base::Unretained(sensor_provider_proxy_.get())));

  media::VideoDecodePerfHistory::SaveCallback save_stats_cb;
  if (GetSiteInstance()->GetBrowserContext()->GetVideoDecodePerfHistory()) {
    save_stats_cb = GetSiteInstance()
                        ->GetBrowserContext()
                        ->GetVideoDecodePerfHistory()
                        ->GetSaveCallback();
  }

  registry_->AddInterface(base::BindRepeating(
      &media::MediaMetricsProvider::Create, frame_tree_node_->IsMainFrame(),
      base::BindRepeating(
          &RenderFrameHostDelegate::GetUkmSourceIdForLastCommittedSource,
          base::Unretained(delegate_)),
      std::move(save_stats_cb)));

  if (base::CommandLine::ForCurrentProcess()->HasSwitch(
          cc::switches::kEnableGpuBenchmarking)) {
    registry_->AddInterface(
        base::Bind(&InputInjectorImpl::Create, weak_ptr_factory_.GetWeakPtr()));
  }

  registry_->AddInterface(base::BindRepeating(
      &QuotaDispatcherHost::CreateForFrame, GetProcess(), routing_id_));

  registry_->AddInterface(
      base::BindRepeating(SpeechRecognitionDispatcherHost::Create,
                          GetProcess()->GetID(), routing_id_),
      base::CreateSingleThreadTaskRunnerWithTraits({BrowserThread::IO}));

  file_system_manager_.reset(new FileSystemManagerImpl(
      GetProcess()->GetID(), routing_id_,
      GetProcess()->GetStoragePartition()->GetFileSystemContext(),
      ChromeBlobStorageContext::GetFor(GetProcess()->GetBrowserContext())));
  registry_->AddInterface(
      base::BindRepeating(&FileSystemManagerImpl::BindRequest,
                          base::Unretained(file_system_manager_.get())),
      base::CreateSingleThreadTaskRunnerWithTraits({BrowserThread::IO}));

  if (Portal::IsEnabled()) {
    registry_->AddInterface(base::BindRepeating(IgnoreResult(&Portal::Create),
                                                base::Unretained(this)));
  }

  registry_->AddInterface(base::BindRepeating(
      &BackgroundFetchServiceImpl::CreateForFrame, GetProcess(), routing_id_));

  registry_->AddInterface(base::BindRepeating(&ContactsManagerImpl::Create));

  registry_->AddInterface(
      base::BindRepeating(&FileChooserImpl::Create, base::Unretained(this)));

  registry_->AddInterface(base::BindRepeating(&AudioContextManagerImpl::Create,
                                              base::Unretained(this)));

  registry_->AddInterface(base::BindRepeating(&WakeLockServiceImpl::Create,
                                              base::Unretained(this)));
}
