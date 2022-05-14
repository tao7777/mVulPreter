DataReductionProxyConfigServiceClient::DataReductionProxyConfigServiceClient(
    const net::BackoffEntry::Policy& backoff_policy,
    DataReductionProxyRequestOptions* request_options,
    DataReductionProxyMutableConfigValues* config_values,
    DataReductionProxyConfig* config,
    DataReductionProxyIOData* io_data,
    network::NetworkConnectionTracker* network_connection_tracker,
    ConfigStorer config_storer)
    : request_options_(request_options),
      config_values_(config_values),
      config_(config),
      io_data_(io_data),
      network_connection_tracker_(network_connection_tracker),
      config_storer_(config_storer),
      backoff_policy_(backoff_policy),
      backoff_entry_(&backoff_policy_),
      config_service_url_(util::AddApiKeyToUrl(params::GetConfigServiceURL())),
      enabled_(false),
      remote_config_applied_(false),
#if defined(OS_ANDROID)
      foreground_fetch_pending_(false),
#endif
      previous_request_failed_authentication_(false),
      failed_attempts_before_success_(0),
      fetch_in_progress_(false),
      client_config_override_used_(false) {
  DCHECK(request_options);
  DCHECK(config_values);
   DCHECK(config);
   DCHECK(io_data);
   DCHECK(config_service_url_.is_valid());
 
   const base::CommandLine& command_line =
       *base::CommandLine::ForCurrentProcess();
  client_config_override_ = command_line.GetSwitchValueASCII(
      switches::kDataReductionProxyServerClientConfig);

  thread_checker_.DetachFromThread();
}
