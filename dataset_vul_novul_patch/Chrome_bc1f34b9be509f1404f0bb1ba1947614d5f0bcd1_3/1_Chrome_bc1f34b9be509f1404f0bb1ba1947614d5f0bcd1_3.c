ServiceManagerContext::ServiceManagerContext() {
  service_manager::mojom::ServiceRequest packaged_services_request;
  if (service_manager::ServiceManagerIsRemote()) {
    auto invitation =
        mojo::edk::IncomingBrokerClientInvitation::AcceptFromCommandLine(
            mojo::edk::TransportProtocol::kLegacy);
    packaged_services_request =
        service_manager::GetServiceRequestFromCommandLine(invitation.get());
  } else {
    std::unique_ptr<BuiltinManifestProvider> manifest_provider =
        base::MakeUnique<BuiltinManifestProvider>();

    static const struct ManifestInfo {
      const char* name;
      int resource_id;
    } kManifests[] = {
        {mojom::kBrowserServiceName, IDR_MOJO_CONTENT_BROWSER_MANIFEST},
        {mojom::kGpuServiceName, IDR_MOJO_CONTENT_GPU_MANIFEST},
        {mojom::kPackagedServicesServiceName,
         IDR_MOJO_CONTENT_PACKAGED_SERVICES_MANIFEST},
        {mojom::kPluginServiceName, IDR_MOJO_CONTENT_PLUGIN_MANIFEST},
        {mojom::kRendererServiceName, IDR_MOJO_CONTENT_RENDERER_MANIFEST},
        {mojom::kUtilityServiceName, IDR_MOJO_CONTENT_UTILITY_MANIFEST},
        {catalog::mojom::kServiceName, IDR_MOJO_CATALOG_MANIFEST},
    };

    for (size_t i = 0; i < arraysize(kManifests); ++i) {
      manifest_provider->AddServiceManifest(kManifests[i].name,
                                            kManifests[i].resource_id);
    }
    for (const auto& manifest :
         GetContentClient()->browser()->GetExtraServiceManifests()) {
      manifest_provider->AddServiceManifest(manifest.name,
                                            manifest.resource_id);
    }
    in_process_context_ = new InProcessServiceManagerContext;

    service_manager::mojom::ServicePtr packaged_services_service;
    packaged_services_request = mojo::MakeRequest(&packaged_services_service);
    in_process_context_->Start(packaged_services_service.PassInterface(),
                               std::move(manifest_provider));
  }

  packaged_services_connection_ = ServiceManagerConnection::Create(
      std::move(packaged_services_request),
      BrowserThread::GetTaskRunnerForThread(BrowserThread::IO));

  service_manager::mojom::ServicePtr root_browser_service;
  ServiceManagerConnection::SetForProcess(ServiceManagerConnection::Create(
      mojo::MakeRequest(&root_browser_service),
      BrowserThread::GetTaskRunnerForThread(BrowserThread::IO)));
  auto* browser_connection = ServiceManagerConnection::GetForProcess();

  service_manager::mojom::PIDReceiverPtr pid_receiver;
  packaged_services_connection_->GetConnector()->StartService(
      service_manager::Identity(mojom::kBrowserServiceName,
                                service_manager::mojom::kRootUserID),
      std::move(root_browser_service), mojo::MakeRequest(&pid_receiver));
  pid_receiver->SetPID(base::GetCurrentProcId());

  service_manager::EmbeddedServiceInfo device_info;
#if defined(OS_ANDROID)
  JNIEnv* env = base::android::AttachCurrentThread();
  base::android::ScopedJavaGlobalRef<jobject> java_nfc_delegate;
  java_nfc_delegate.Reset(Java_ContentNfcDelegate_create(env));
  DCHECK(!java_nfc_delegate.is_null());

  device_info.factory =
      base::Bind(&device::CreateDeviceService,
                 BrowserThread::GetTaskRunnerForThread(BrowserThread::FILE),
                 BrowserThread::GetTaskRunnerForThread(BrowserThread::IO),
                 base::Bind(&WakeLockContextHost::GetNativeViewForContext),
                 std::move(java_nfc_delegate));
#else
  device_info.factory =
      base::Bind(&device::CreateDeviceService,
                 BrowserThread::GetTaskRunnerForThread(BrowserThread::FILE),
                 BrowserThread::GetTaskRunnerForThread(BrowserThread::IO));
#endif
  device_info.task_runner = base::ThreadTaskRunnerHandle::Get();
  packaged_services_connection_->AddEmbeddedService(device::mojom::kServiceName,
                                                    device_info);

  if (base::FeatureList::IsEnabled(features::kGlobalResourceCoordinator)) {
    service_manager::EmbeddedServiceInfo resource_coordinator_info;
    resource_coordinator_info.factory =
        base::Bind(&resource_coordinator::ResourceCoordinatorService::Create);
    packaged_services_connection_->AddEmbeddedService(
        resource_coordinator::mojom::kServiceName, resource_coordinator_info);
  }

  ContentBrowserClient::StaticServiceMap services;
  GetContentClient()->browser()->RegisterInProcessServices(&services);
  for (const auto& entry : services) {
    packaged_services_connection_->AddEmbeddedService(entry.first,
                                                      entry.second);
  }

  g_io_thread_connector.Get() = browser_connection->GetConnector()->Clone();

  ContentBrowserClient::OutOfProcessServiceMap out_of_process_services;
  GetContentClient()->browser()->RegisterOutOfProcessServices(
      &out_of_process_services);

  out_of_process_services[data_decoder::mojom::kServiceName] = {
      base::ASCIIToUTF16("Data Decoder Service"), SANDBOX_TYPE_UTILITY};

  bool network_service_enabled =
      base::FeatureList::IsEnabled(features::kNetworkService);
  if (network_service_enabled) {
    out_of_process_services[content::mojom::kNetworkServiceName] = {
        base::ASCIIToUTF16("Network Service"), SANDBOX_TYPE_NETWORK};
  }

  if (base::FeatureList::IsEnabled(video_capture::kMojoVideoCapture)) {
    out_of_process_services[video_capture::mojom::kServiceName] = {
        base::ASCIIToUTF16("Video Capture Service"), SANDBOX_TYPE_NO_SANDBOX};
   }
 
 #if BUILDFLAG(ENABLE_MOJO_MEDIA_IN_UTILITY_PROCESS)
  out_of_process_services[media::mojom::kMediaServiceName] = {
      base::ASCIIToUTF16("Media Service"), SANDBOX_TYPE_NO_SANDBOX};
 #endif
 
   for (const auto& service : out_of_process_services) {
    packaged_services_connection_->AddServiceRequestHandler(
        service.first, base::Bind(&StartServiceInUtilityProcess, service.first,
                                  service.second.first, service.second.second));
  }

#if BUILDFLAG(ENABLE_MOJO_MEDIA_IN_GPU_PROCESS)
  packaged_services_connection_->AddServiceRequestHandler(
      media::mojom::kMediaServiceName,
      base::Bind(&StartServiceInGpuProcess, media::mojom::kMediaServiceName));
#endif

  packaged_services_connection_->AddServiceRequestHandler(
      shape_detection::mojom::kServiceName,
      base::Bind(&StartServiceInGpuProcess,
                 shape_detection::mojom::kServiceName));

  packaged_services_connection_->Start();

  RegisterCommonBrowserInterfaces(browser_connection);
  browser_connection->Start();

  if (network_service_enabled) {
    browser_connection->GetConnector()->StartService(
        mojom::kNetworkServiceName);
  }
}
