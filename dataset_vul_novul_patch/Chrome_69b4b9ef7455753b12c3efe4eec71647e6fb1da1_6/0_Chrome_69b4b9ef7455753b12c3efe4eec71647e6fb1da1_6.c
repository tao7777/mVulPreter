DataReductionProxyIOData::DataReductionProxyIOData(
    Client client,
    PrefService* prefs,
    network::NetworkConnectionTracker* network_connection_tracker,
    scoped_refptr<base::SingleThreadTaskRunner> io_task_runner,
    scoped_refptr<base::SingleThreadTaskRunner> ui_task_runner,
    bool enabled,
    const std::string& user_agent,
    const std::string& channel)
    : client_(client),
      network_connection_tracker_(network_connection_tracker),
      io_task_runner_(io_task_runner),
      ui_task_runner_(ui_task_runner),
      enabled_(enabled),
      channel_(channel),
      effective_connection_type_(net::EFFECTIVE_CONNECTION_TYPE_UNKNOWN) {
  DCHECK(io_task_runner_);
  DCHECK(ui_task_runner_);
  configurator_.reset(new DataReductionProxyConfigurator());
  configurator_->SetConfigUpdatedCallback(base::BindRepeating(
      &DataReductionProxyIOData::OnProxyConfigUpdated, base::Unretained(this)));
  DataReductionProxyMutableConfigValues* raw_mutable_config = nullptr;
    std::unique_ptr<DataReductionProxyMutableConfigValues> mutable_config =
        std::make_unique<DataReductionProxyMutableConfigValues>();
    raw_mutable_config = mutable_config.get();
    config_.reset(new DataReductionProxyConfig(
        io_task_runner, ui_task_runner, network_connection_tracker_,
        std::move(mutable_config), configurator_.get()));
    request_options_.reset(
        new DataReductionProxyRequestOptions(client_, config_.get()));
    request_options_->Init();
    request_options_->SetUpdateHeaderCallback(base::BindRepeating(
        &DataReductionProxyIOData::UpdateProxyRequestHeaders,
        base::Unretained(this)));

    if (!params::IsIncludedInHoldbackFieldTrial()) {
      config_client_.reset(new DataReductionProxyConfigServiceClient(
          GetBackoffPolicy(), request_options_.get(), raw_mutable_config,
          config_.get(), this, network_connection_tracker_,
          base::BindRepeating(&DataReductionProxyIOData::StoreSerializedConfig,
                              base::Unretained(this))));
    }
 
     network_properties_manager_.reset(new NetworkPropertiesManager(
         base::DefaultClock::GetInstance(), prefs, ui_task_runner_));
}
