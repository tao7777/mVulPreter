 void DataReductionProxyConfigServiceClient::RetrieveRemoteConfig() {
   DCHECK(thread_checker_.CalledOnValidThread());
   CreateClientConfigRequest request;
   std::string serialized_request;
 #if defined(OS_ANDROID)
  request.set_telephony_network_operator(
      net::android::GetTelephonyNetworkOperator());
#endif

  data_reduction_proxy::ConfigDeviceInfo* device_info =
      request.mutable_device_info();
  device_info->set_total_device_memory_kb(
      base::SysInfo::AmountOfPhysicalMemory() / 1024);
  const std::string& session_key = request_options_->GetSecureSession();
  if (!session_key.empty())
    request.set_session_key(request_options_->GetSecureSession());
  request.set_dogfood_group(
      base::FeatureList::IsEnabled(features::kDogfood)
          ? CreateClientConfigRequest_DogfoodGroup_DOGFOOD
          : CreateClientConfigRequest_DogfoodGroup_NONDOGFOOD);
  data_reduction_proxy::VersionInfo* version_info =
      request.mutable_version_info();
  uint32_t build;
  uint32_t patch;
  util::GetChromiumBuildAndPatchAsInts(util::ChromiumVersion(), &build, &patch);
  version_info->set_client(util::GetStringForClient(io_data_->client()));
  version_info->set_build(build);
  version_info->set_patch(patch);
  version_info->set_channel(io_data_->channel());
  request.SerializeToString(&serialized_request);

  net::NetworkTrafficAnnotationTag traffic_annotation =
      net::DefineNetworkTrafficAnnotation("data_reduction_proxy_config", R"(
        semantics {
          sender: "Data Reduction Proxy"
          description:
            "Requests a configuration that specifies how to connect to the "
            "data reduction proxy."
          trigger:
            "Requested when Data Saver is enabled and the browser does not "
            "have a configuration that is not older than a threshold set by "
            "the server."
          data: "None."
          destination: GOOGLE_OWNED_SERVICE
        }
        policy {
          cookies_allowed: NO
          setting:
            "Users can control Data Saver on Android via 'Data Saver' setting. "
            "Data Saver is not available on iOS, and on desktop it is enabled "
            "by insalling the Data Saver extension."
          policy_exception_justification: "Not implemented."
        })");
  fetch_in_progress_ = true;

  auto resource_request = std::make_unique<network::ResourceRequest>();
  resource_request->url = config_service_url_;
  resource_request->method = "POST";
  resource_request->load_flags = net::LOAD_BYPASS_PROXY;
  resource_request->allow_credentials = false;
  url_loader_ = variations::CreateSimpleURLLoaderWithVariationsHeader(
      std::move(resource_request), variations::InIncognito::kNo,
      variations::SignedIn::kNo, traffic_annotation);

  url_loader_->AttachStringForUpload(serialized_request,
                                     "application/x-protobuf");
  static const int kMaxRetries = 5;
  url_loader_->SetRetryOptions(
      kMaxRetries, network::SimpleURLLoader::RETRY_ON_NETWORK_CHANGE);
  url_loader_->DownloadToStringOfUnboundedSizeUntilCrashAndDie(
      url_loader_factory_.get(),
      base::BindOnce(&DataReductionProxyConfigServiceClient::OnURLLoadComplete,
                     base::Unretained(this)));
}
