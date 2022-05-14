 void DataReductionProxyConfig::FetchWarmupProbeURL() {
   DCHECK(thread_checker_.CalledOnValidThread());
 
   if (!enabled_by_user_) {
     RecordWarmupURLFetchAttemptEvent(
         WarmupURLFetchAttemptEvent::kProxyNotEnabledByUser);
    return;
  }

  if (!params::FetchWarmupProbeURLEnabled()) {
    RecordWarmupURLFetchAttemptEvent(
        WarmupURLFetchAttemptEvent::kWarmupURLFetchingDisabled);
    return;
  }

  if (connection_type_ == network::mojom::ConnectionType::CONNECTION_NONE) {
    RecordWarmupURLFetchAttemptEvent(
        WarmupURLFetchAttemptEvent::kConnectionTypeNone);
    return;
  }

  base::Optional<DataReductionProxyServer> warmup_proxy =
      GetProxyConnectionToProbe();

  if (!warmup_proxy)
    return;

  warmup_url_fetch_in_flight_secure_proxy_ = warmup_proxy->IsSecureProxy();
  warmup_url_fetch_in_flight_core_proxy_ = warmup_proxy->IsCoreProxy();

  size_t previous_attempt_counts = GetWarmupURLFetchAttemptCounts();

  network_properties_manager_->OnWarmupFetchInitiated(
      warmup_url_fetch_in_flight_secure_proxy_,
      warmup_url_fetch_in_flight_core_proxy_);

  RecordWarmupURLFetchAttemptEvent(WarmupURLFetchAttemptEvent::kFetchInitiated);

  warmup_url_fetcher_->FetchWarmupURL(previous_attempt_counts,
                                      warmup_proxy.value());
}
