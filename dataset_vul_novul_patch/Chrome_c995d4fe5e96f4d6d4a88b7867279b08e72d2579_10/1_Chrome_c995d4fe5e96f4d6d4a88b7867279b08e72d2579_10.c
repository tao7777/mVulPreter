 void DataReductionProxySettings::SetDataReductionProxyEnabled(bool enabled) {
   DCHECK(thread_checker_.CalledOnValidThread());
   DCHECK(data_reduction_proxy_service_->compression_stats());
  if (spdy_proxy_auth_enabled_.GetValue() != enabled) {
    spdy_proxy_auth_enabled_.SetValue(enabled);
     OnProxyEnabledPrefChange();
 #if defined(OS_ANDROID)
     data_reduction_proxy_service_->compression_stats()
        ->SetDataUsageReportingEnabled(enabled);
#endif  // defined(OS_ANDROID)
  }
}
