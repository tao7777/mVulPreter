void DataReductionProxyConfig::InitializeOnIOThread(
    scoped_refptr<network::SharedURLLoaderFactory> url_loader_factory,
    WarmupURLFetcher::CreateCustomProxyConfigCallback
        create_custom_proxy_config_callback,
    NetworkPropertiesManager* manager,
    const std::string& user_agent) {
  DCHECK(thread_checker_.CalledOnValidThread());
   network_properties_manager_ = manager;
   network_properties_manager_->ResetWarmupURLFetchMetrics();
 
  secure_proxy_checker_.reset(new SecureProxyChecker(url_loader_factory));
  warmup_url_fetcher_.reset(new WarmupURLFetcher(
      create_custom_proxy_config_callback,
      base::BindRepeating(
          &DataReductionProxyConfig::HandleWarmupFetcherResponse,
          base::Unretained(this)),
      base::BindRepeating(&DataReductionProxyConfig::GetHttpRttEstimate,
                          base::Unretained(this)),
      ui_task_runner_, user_agent));
 
   AddDefaultProxyBypassRules();
 
  network_connection_tracker_->AddNetworkConnectionObserver(this);
  network_connection_tracker_->GetConnectionType(
      &connection_type_,
      base::BindOnce(&DataReductionProxyConfig::OnConnectionChanged,
                     weak_factory_.GetWeakPtr()));
}
