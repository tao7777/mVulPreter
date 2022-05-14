void UtilityServiceFactory::RegisterServices(ServiceMap* services) {
  GetContentClient()->utility()->RegisterServices(services);

  service_manager::EmbeddedServiceInfo video_capture_info;
  video_capture_info.factory = base::Bind(&CreateVideoCaptureService);
  services->insert(
      std::make_pair(video_capture::mojom::kServiceName, video_capture_info));
 
 #if BUILDFLAG(ENABLE_PEPPER_CDMS)
   service_manager::EmbeddedServiceInfo info;
  info.factory = base::Bind(&CreateCdmService);
  services->insert(std::make_pair(media::mojom::kCdmServiceName, info));
 #endif
 
   service_manager::EmbeddedServiceInfo shape_detection_info;
  shape_detection_info.factory =
      base::Bind(&shape_detection::ShapeDetectionService::Create);
  services->insert(std::make_pair(shape_detection::mojom::kServiceName,
                                  shape_detection_info));

  service_manager::EmbeddedServiceInfo data_decoder_info;
  data_decoder_info.factory = base::Bind(&CreateDataDecoderService);
  services->insert(
      std::make_pair(data_decoder::mojom::kServiceName, data_decoder_info));

  if (base::FeatureList::IsEnabled(features::kNetworkService)) {
    GetContentClient()->utility()->RegisterNetworkBinders(
        network_registry_.get());
    service_manager::EmbeddedServiceInfo network_info;
    network_info.factory = base::Bind(
        &UtilityServiceFactory::CreateNetworkService, base::Unretained(this));
    network_info.task_runner = ChildProcess::current()->io_task_runner();
    services->insert(
        std::make_pair(content::mojom::kNetworkServiceName, network_info));
  }
}
